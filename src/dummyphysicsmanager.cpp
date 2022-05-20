#include "dummyphysicsmanager.hpp"

DummyPhysicsManager::DummyPhysicsManager()
{

}

DummyPhysicsManager::~DummyPhysicsManager()
{

}

void DummyPhysicsManager::init()
{

}

void DummyPhysicsManager::update(double dt)
{
    (void)dt;
}

void DummyPhysicsManager::clear()
{

}

void DummyPhysicsManager::setRaycastCallback(OnRaycastHitCallback callb)
{
    (void)callb;
}

bool DummyPhysicsManager::raycast(const glm::vec3 &pos, const glm::vec3 &dir, float len)
{
    (void)pos;
    (void)dir;
    (void)len;
    return false;
}

void DummyPhysicsManager::createBody(const PhysicsBodyCreateInfo &cInfo, Entity *parent)
{
    (void)cInfo;
    (void)parent;
}
