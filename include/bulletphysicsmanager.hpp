#ifndef BULLETPHYSICSMANAGER_HPP
#define BULLETPHYSICSMANAGER_HPP

#include "physicsmanager.hpp"

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

    void resetWorld();

    void update(double dt) override;

    void createBody(const PhysicsBodyCreateInfo &cInfo, Entity *parent) override;

private:
    btCollisionConfiguration *m_collisionConfig;
    btDispatcher *m_dispatcher;
    btBroadphaseInterface *m_pairCache;
#ifdef BULLET_MULTITHREADED
    std::vector<btConstraintSolver*> m_solvers;
    btConstraintSolverPoolMt *m_solverPool;
#endif
    btConstraintSolver *m_solver;
    //
    btDynamicsWorld *m_world;

    std::vector<btRigidBody*> m_bodies;
};

#endif // BULLETPHYSICSMANAGER_HPP
