#include "bulletphysicsmanager.hpp"
#include "entity.hpp"

#include "servicelocator.hpp"

static const char *MODULE_NAME = "BulletPhysicsManager";

BulletPhysicsManager *BulletPhysicsManager::p_shared = nullptr;

BulletPhysicsManager::BulletPhysicsManager()
    : m_collisionConfig(nullptr),
      m_dispatcher(nullptr),
      m_pairCache(nullptr),
      m_solver(nullptr),
      m_world(nullptr),
      m_raycastHitCallback(nullptr)
{
    BulletPhysicsManager::p_shared = this;
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

    gContactStartedCallback = BulletPhysicsManager::OnContactBegin;

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

void BulletPhysicsManager::clear()
{
    resetWorld();
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
    m_world->stepSimulation(dt, 1);
    for(auto &body : m_bodies)
    {
        if(!body->isActive())
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
        body->setFriction(props.getFriction());

        if(cInfo.getMass() > 0)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

        const glm::vec3 &startImpulse = cInfo.getImpulse();
        if(glm::length(startImpulse) > 0.f)
            body->applyCentralImpulse(btVector3(startImpulse.x, startImpulse.y, startImpulse.z));

    }
    m_world->addRigidBody(body);
    m_bodies.push_back(body);
}

void BulletPhysicsManager::setRaycastCallback(OnRaycastHitCallback callb)
{
    m_raycastHitCallback = callb;
}

bool BulletPhysicsManager::raycast(const glm::vec3 &pos, const glm::vec3 &dir, float len)
{
    btVector3 fromWorld = btVector3(pos.x, pos.y, pos.z),
              toWorld   = btVector3(pos.x+dir.x*len, pos.y+dir.y*len, pos.z+dir.z*len);
    btCollisionWorld::ClosestRayResultCallback rayCallback(fromWorld, toWorld);
    m_world->rayTest(fromWorld, toWorld, rayCallback);

    bool hasHit = rayCallback.hasHit();
    if(hasHit)
    {
        if(m_raycastHitCallback)
        {
            const btRigidBody *body = btRigidBody::upcast(rayCallback.m_collisionObject);

            btVector3 hitPos = rayCallback.m_hitPointWorld;

            m_raycastHitCallback(static_cast<Entity*>(body->getUserPointer()),
                                 PhysicsRaycastData{glm::vec3(hitPos.x(), hitPos.y(), hitPos.z())
                                 });
        }
    }
    return hasHit;
}

void BulletPhysicsManager::OnContactBegin(btPersistentManifold * const &manifold)
{
    (void)manifold;
    /*
    if(manifold->getBody0()->getUserPointer())
    {
        const btRigidBody *body = btRigidBody::upcast(manifold->getBody0());
        Entity *ent = static_cast<Entity*>(manifold->getBody0()->getUserPointer());
        btVector3 contactPos = manifold->getContactPoint(0).getPositionWorldOnA();
        btVector3 velocity = btRigidBody::upcast(manifold->getBody0())->getLinearVelocity();
        ent->OnContactBegin({glm::vec3(contactPos.x(),contactPos.y(),contactPos.z()),
                             glm::vec3(velocity.x(), velocity.y(), velocity.z()),
                             body->getMass()});
    }
    */
}

