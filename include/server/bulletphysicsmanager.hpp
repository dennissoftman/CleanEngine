#ifndef BULLETPHYSICSMANAGER_HPP
#define BULLETPHYSICSMANAGER_HPP

#include "server/physicsmanager.hpp"

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#ifdef BULLET_MULTITHREADED
#include <bullet/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h>
#include <bullet/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h>

#include <thread>
#endif

class BulletPhysicsManager : public PhysicsManager
{
public:
    BulletPhysicsManager();
    ~BulletPhysicsManager() override;

    void init() override;
    void terminate();

    void clear() override;
    void resetWorld();

    void update(double dt) override;

    void createBody(const PhysicsBodyCreateInfo &cInfo, Entity *parent) override;

    void setRaycastCallback(OnRaycastHitCallback callb) override;
    bool raycast(const glm::vec3 &pos, const glm::vec3 &dir, float len) override;

    static void OnContactBegin(btPersistentManifold* const& manifold);
    static void OnRaycastHit(btCollisionWorld::ClosestRayResultCallback &rayCallback);

    static BulletPhysicsManager *p_shared;
private:
    btCollisionConfiguration *m_collisionConfig;
    btCollisionDispatcher *m_dispatcher;
    btBroadphaseInterface *m_pairCache;
#ifdef BULLET_MULTITHREADED
    std::vector<btConstraintSolver*> m_solvers;
    btConstraintSolverPoolMt *m_solverPool;
#endif
    btConstraintSolver *m_solver;
    //
    btDynamicsWorld *m_world;

    OnRaycastHitCallback m_raycastHitCallback;

    std::vector<btRigidBody*> m_bodies;
};

#endif // BULLETPHYSICSMANAGER_HPP
