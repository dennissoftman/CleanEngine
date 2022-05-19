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

void DummyPhysicsManager::createBody(const PhysicsBodyCreateInfo &cInfo, Entity *parent)
{
    (void)cInfo;
    (void)parent;
}
