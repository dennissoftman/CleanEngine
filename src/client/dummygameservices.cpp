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

std::vector<FriendDetails> DummyGameServices::getFriends() const
{
    return std::vector<FriendDetails>{};
}