#include "client/dummygameclient.hpp"
#include "common/servicelocator.hpp"

GameClient *GameClient::corePtr = nullptr;

GameClient *GameClient::create()
{
    return new DummyGameClient();
}

DummyGameClient::DummyGameClient()
{
    GameClient::corePtr = this;
}

void DummyGameClient::init()
{
    ServiceLocator::getScriptEngine().init();
}

void DummyGameClient::run()
{
    int64_t n = 0;
    int sw = 0;
    do
    {
        if(sw)
            ++n;
        else
            --n;

        if(n >= 0xdeadb00b)
            sw = 0;
        if(n <= 0xdead)
            sw = 1;
    } while(n < 0xdeadbeef);
}

void DummyGameClient::terminate()
{

}

double DummyGameClient::getDeltaTime() const
{
    return 1;
}

double DummyGameClient::getElapsedTime() const
{
    return 0;
}

void DummyGameClient::lockCursor()
{

}

void DummyGameClient::unlockCursor()
{

}
