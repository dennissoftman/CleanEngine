#ifndef PHYSICSMANAGER_HPP
#define PHYSICSMANAGER_HPP

class PhysicsManager
{
public:
    virtual ~PhysicsManager() {}

    virtual void init() = 0;

    virtual void update(double dt) = 0;
};

#endif
