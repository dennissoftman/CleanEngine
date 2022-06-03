#ifndef DUMMYPHYSICSMANAGER_HPP
#define DUMMYPHYSICSMANAGER_HPP

#include "server/physicsmanager.hpp"

class DummyPhysicsManager : public PhysicsManager
{
public:
    DummyPhysicsManager();
    ~DummyPhysicsManager() override;

    void init() override;
    void update(double dt) override;

    void clear() override;

    void setRaycastCallback(OnRaycastHitCallback callb) override;
    bool raycast(const glm::vec3 &pos, const glm::vec3 &dir, float len) override;

    void createBody(const PhysicsBodyCreateInfo &cInfo, Entity *parent = nullptr) override;
};

#endif // DUMMYPHYSICSMANAGER_HPP
