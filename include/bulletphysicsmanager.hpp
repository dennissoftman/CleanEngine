#ifndef BULLETPHYSICSMANAGER_HPP
#define BULLETPHYSICSMANAGER_HPP

#include "physicsmanager.hpp"

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

class BulletPhysicsManager : public PhysicsManager
{
public:
    BulletPhysicsManager();
    ~BulletPhysicsManager() override;

    void init() override;
    void terminate();

    void update(double dt) override;

private:
    btCollisionConfiguration *m_collisionConfig;
    btDispatcher *m_dispatcher;
    btBroadphaseInterface *m_pairCache;
    btConstraintSolver *m_solver;
    //
    btDynamicsWorld *m_world;
};

#endif // BULLETPHYSICSMANAGER_HPP
