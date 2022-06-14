#include "common/dummyservices.hpp"

GameServices *GameServices::create()
{
    return new DummyServices();
}

DummyServices::DummyServices()
{

}

DummyServices::~DummyServices()
{

}

void DummyServices::init()
{

}

void DummyServices::authorize()
{

}
