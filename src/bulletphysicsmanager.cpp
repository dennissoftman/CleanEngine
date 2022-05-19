#include "bulletphysicsmanager.hpp"
#include "servicelocator.hpp"

static const char *MODULE_NAME = "BulletPhysicsManager";

BulletPhysicsManager::BulletPhysicsManager()
    : m_collisionConfig(nullptr),
      m_dispatcher(nullptr),
      m_pairCache(nullptr),
      m_solver(nullptr),
      m_world(nullptr)
{

}

BulletPhysicsManager::~BulletPhysicsManager()
{
    terminate();
}

void BulletPhysicsManager::init()
{
    Logger &logger = ServiceLocator::getLogger();

    m_collisionConfig = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfig);
    m_pairCache = new btDbvtBroadphase();

#ifdef BULLET_MULTITHREADED
    btITaskScheduler *scheduler = nullptr;

    if(scheduler = btGetOpenMPTaskScheduler())
        logger.info(MODULE_NAME, "Using OpenMP task scheduler");
    else if(scheduler = btGetTBBTaskScheduler())
        logger.info(MODULE_NAME, "Using Intel TBB task scheduler");
    else if(scheduler = btGetPPLTaskScheduler())
        logger.info(MODULE_NAME, "Using Microsoft PPL task scheduler");
    else if(scheduler = btCreateDefaultTaskScheduler())
        logger.info(MODULE_NAME, "Using threads task scheduler");
    else
    {
        scheduler = btGetSequentialTaskScheduler();
        logger.info(MODULE_NAME, "Multithreading not available, falling down to one thread");
    }

    if(!scheduler)
    {
        logger.error(MODULE_NAME, "Failed to create task scheduler");
        return terminate();
    }
    btSetTaskScheduler(scheduler);

    unsigned int thread_count = std::thread::hardware_concurrency();
    for(unsigned int i=0; i < thread_count; i++)
    {

        m_solvers.push_back(new btSequentialImpulseConstraintSolver());
    }

    m_solverPool = new btConstraintSolverPoolMt(m_solvers.data(), m_solvers.size());
    m_solver = new btSequentialImpulseConstraintSolverMt();

    m_world = new btDiscreteDynamicsWorldMt(m_dispatcher, m_pairCache, m_solverPool, m_solver, m_collisionConfig);
#else
    m_solver = new btSequentialImpulseConstraintSolver();
    m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_pairCache, m_solver, m_collisionConfig);
#endif

    m_world->setGravity(btVector3(0, -9.8f, 0));

    ServiceLocator::getLogger().info(MODULE_NAME, "Physics init completed");
}

void BulletPhysicsManager::terminate()
{
    resetWorld();

    delete m_world;
    m_world = nullptr;
    delete m_solver;
    m_solver = nullptr;
    delete m_pairCache;
    m_pairCache = nullptr;
    delete m_dispatcher;
    m_dispatcher = nullptr;
    delete m_collisionConfig;
    m_collisionConfig = nullptr;
}

void BulletPhysicsManager::resetWorld()
{
    for(auto &body : m_bodies)
    {
        m_world->removeRigidBody(body);
        delete body->getMotionState();
        delete body->getCollisionShape();
        delete body;
    }
    m_bodies.clear();
}

void BulletPhysicsManager::update(double dt)
{
    // m_world->stepSimulation(dt, 1);
    m_world->stepSimulation(dt, 3);
    for(auto &body : m_bodies)
    {
        if(!body->isActive()) // little optimization
            continue;

        void *ptr = body->getUserPointer();
        if(ptr)
        {
            const btTransform &t = body->getWorldTransform();
            const btVector3 &pos = t.getOrigin();
            const btQuaternion &rot = t.getRotation();


            Entity *ent = static_cast<Entity*>(ptr);
            ent->setPosition(glm::vec3(pos.x(), pos.y(), pos.z()));
            ent->setRotation(glm::quat(rot.w(), rot.x(), rot.y(), rot.z()));
        }
    }
}

void BulletPhysicsManager::createBody(const PhysicsBodyCreateInfo &cInfo, Entity *parent)
{
    const auto &shapeInfo = cInfo.getShapeInfo();
    btCollisionShape *bodyShape = nullptr;
    switch(shapeInfo.getShapeType())
    {
    case PhysicsShape::eBox:
    {
        glm::vec3 extents = std::get<glm::vec3>(shapeInfo.getShapeData());
        bodyShape = new btBoxShape(btVector3(extents.x, extents.y, extents.z));
        break;
    }
    case PhysicsShape::eSphere:
    {
        float radius = std::get<float>(shapeInfo.getShapeData());
        bodyShape = new btSphereShape(radius);
        break;
    }
    case PhysicsShape::eTriangleMesh:
    {
        ServiceLocator::getLogger().warning(MODULE_NAME, "TriangleMesh shapes unsupported");
        return;
        break;
    }
    default:
        return;
    }

    btMotionState *bodyMotionState = new btDefaultMotionState();
    if(parent)
    {
        btTransform t{};
        const glm::vec3 &pos = parent->getPosition();
        const glm::quat &rot = parent->getRotation();
        t.setOrigin(btVector3(pos.x, pos.y, pos.z));
        t.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

        bodyMotionState->setWorldTransform(t);
    }

    btVector3 bodyInertia{};
    if(cInfo.getMass() > 0)
        bodyShape->calculateLocalInertia(cInfo.getMass(), bodyInertia);

    btRigidBody *body = new btRigidBody(cInfo.getMass(), bodyMotionState, bodyShape, bodyInertia);
    body->setUserPointer(parent);
    { // apply body modifiers
        const PhysicsBodyProperties &props = cInfo.getBodyProperties();
        body->setRestitution(props.getRestitution());

        body->setFriction(1.f);

        const glm::vec3 &startImpulse = cInfo.getImpulse();
        if(glm::length(startImpulse) > 0.f)
            body->applyCentralImpulse(btVector3(startImpulse.x, startImpulse.y, startImpulse.z));

    }
    m_world->addRigidBody(body);
    m_bodies.push_back(body);
}
