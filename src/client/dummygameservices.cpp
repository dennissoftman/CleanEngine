#include "client/dummygameservices.hpp"

GameServices *GameServices::create()
{
    return new DummyGameServices();
}

DummyGameServices::DummyGameServices()
{

}

DummyGameServices::~DummyGameServices()
{

}

void DummyGameServices::init()
{

}

void DummyGameServices::update(double dt)
{

}

void DummyGameServices::authorize()
{

}
