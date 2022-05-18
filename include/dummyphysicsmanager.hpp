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
};

#endif // DUMMYPHYSICSMANAGER_HPP
