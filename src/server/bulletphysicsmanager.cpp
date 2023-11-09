#include "server/bulletphysicsmanager.hpp"
#include "common/servicelocator.hpp"
#include "common/entities/entity.hpp"
#include "common/entities/bulletbodycomponent.hpp"
#include <format>

static const char *MODULE_NAME = "BulletPhysicsManager";

BulletPhysicsManager *BulletPhysicsManager::p_shared = nullptr;

PhysicsManager *PhysicsManager::create()
{
    return new BulletPhysicsManager();
}

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

    if((scheduler = btGetOpenMPTaskScheduler()))
        logger.info(MODULE_NAME, "Using OpenMP task scheduler");
    else if((scheduler = btGetTBBTaskScheduler()))
        logger.info(MODULE_NAME, "Using Intel TBB task scheduler");
    else if((scheduler = btGetPPLTaskScheduler()))
        logger.info(MODULE_NAME, "Using Microsoft PPL task scheduler");
    else if((scheduler = btCreateDefaultTaskScheduler()))
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
            BulletBodyComponent *comp = static_cast<BulletBodyComponent*>(ptr);

            const btTransform &t = body->getWorldTransform();
            const btVector3 &pos = t.getOrigin();
            const btQuaternion &rot = t.getRotation();

            comp->updateTransform(pos, rot);
        }
    }
}

void *BulletPhysicsManager::createBody(const PhysicsBodyCreateInfo &cInfo, const glm::vec3 &pos, const glm::quat &rot)
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
        return nullptr;
        break;
    }
    default:
        return nullptr;
    }

    btMotionState *bodyMotionState = new btDefaultMotionState();
    btTransform t{};
    t.setOrigin(btVector3(pos.x, pos.y, pos.z));
    t.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));
    bodyMotionState->setWorldTransform(t);

    btVector3 bodyInertia{};
    if(cInfo.getMass() > 0)
        bodyShape->calculateLocalInertia(cInfo.getMass(), bodyInertia);

    btRigidBody *body = new btRigidBody(cInfo.getMass(), bodyMotionState, bodyShape, bodyInertia);
    { // apply body modifiers
        const PhysicsBodyProperties &props = cInfo.getBodyProperties();
        body->setRestitution(props.getRestitution());
        body->setFriction(props.getFriction());

        if(cInfo.getMass() > 0)
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
        else
            body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);

        const glm::vec3 &startImpulse = cInfo.getImpulse();
        if(glm::length(startImpulse) > 0.f)
            body->applyCentralImpulse(btVector3(startImpulse.x, startImpulse.y, startImpulse.z));

    }

    m_world->addRigidBody(body);
    m_bodies.push_back(body);
    return body;
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

void BulletPhysicsManager::explode(const glm::vec3 &pos, float radius, float power)
{
    btVector3 fromWorld = btVector3(pos.x, pos.y, pos.z);
    btVector3 toWorld;

    std::mt19937 generator;
    std::uniform_real_distribution<float> uniform01(0.f, 1.f);
    std::vector<btRigidBody*> touchedBodies;
    for(int i=0; i < 5000; i++)
    {
        // temp
        float theta = 2.f * glm::pi<float>() * uniform01(generator);
        float phi = acos(1.f - 2.f * uniform01(generator));
        float x = sin(phi) * cos(theta);
        float y = sin(phi) * sin(theta);
        float z = cos(phi);
        //
        toWorld = btVector3(pos.x+x*radius, pos.y+y*radius, pos.z+z*radius);
        btCollisionWorld::ClosestRayResultCallback rayCallback(fromWorld, toWorld);
        m_world->rayTest(fromWorld, toWorld, rayCallback);

        bool hasHit = rayCallback.hasHit();
        if(hasHit)
        {
            btRigidBody *body = btRigidBody::upcast((btCollisionObject*)rayCallback.m_collisionObject);
            if(std::find(touchedBodies.begin(), touchedBodies.end(), body) != touchedBodies.end())
            {
                i--;
                continue;
            }
            body->applyCentralImpulse(btVector3(x, y, z) * power);
            touchedBodies.push_back(body);
        }
    }
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

