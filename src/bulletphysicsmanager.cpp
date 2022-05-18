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
    m_collisionConfig = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfig);
    m_pairCache = new btDbvtBroadphase();
    m_solver = new btSequentialImpulseConstraintSolver();

    m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_pairCache, m_solver, m_collisionConfig);
    m_world->setGravity(btVector3(0, -9.8f, 0));

    ServiceLocator::getLogger().info(MODULE_NAME, "Physics init completed");
}

void BulletPhysicsManager::terminate()
{
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

void BulletPhysicsManager::update(double dt)
{

}
