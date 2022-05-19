#ifndef DUMMYPHYSICSMANAGER_HPP
#define DUMMYPHYSICSMANAGER_HPP

#include "physicsmanager.hpp"

class DummyPhysicsManager : public PhysicsManager
{
public:
    DummyPhysicsManager();
    ~DummyPhysicsManager() override;

    void init() override;
    void update(double dt) override;

    void createBody(const PhysicsBodyCreateInfo &cInfo, Entity *parent = nullptr) override;
};

#endif // DUMMYPHYSICSMANAGER_HPP
