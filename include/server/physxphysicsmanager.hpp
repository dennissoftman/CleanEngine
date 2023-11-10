#ifndef PHYSXPHYSICSMANAGER_HPP
#define PHYSXPHYSICSMANAGER_HPP

#include "physicsmanager.hpp"

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>


class PhysXPhysicsManager : public PhysicsManager
{
public:
    PhysXPhysicsManager();
    ~PhysXPhysicsManager() override;

    void init() override;
    void update(double dt) override;
    void clear() override;
    void setRaycastCallback(OnRaycastHitCallback callb) override;
    bool raycast(const glm::vec3 &pos, const glm::vec3 &dir, float len) override;
    void explode(const glm::vec3 &pos, float radius, float power) override;
    void createBody(const PhysicsBodyCreateInfo &cInfo, Entity *parent) override;

private:
    void terminate();

    physx::PxDefaultAllocator m_defaultAllocatorCallback;
    physx::PxDefaultErrorCallback m_defaultErrorCallback;
    physx::PxTolerancesScale m_toleranceScale;

    physx::PxDefaultCpuDispatcher *m_dispatcher;
    physx::PxFoundation *m_foundation;
    physx::PxPhysics *m_physics;
    physx::PxScene *m_scene;
    physx::PxPvdTransport *m_pvdTransport;
    physx::PxPvd *m_pvd;
    physx::PxPvdSceneClient *m_pvdClient;

    std::vector<physx::PxActor*> m_bodies;
};

#endif // PHYSXPHYSICSMANAGER_HPP
