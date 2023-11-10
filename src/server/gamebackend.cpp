#include "server/gamebackend.hpp"
#include "common/servicelocator.hpp"

GameBackend *GameBackend::create()
{
    return new GameBackend();
}

GameBackend::GameBackend()
{
    
}

GameBackend::~GameBackend()
{

}

void GameBackend::init()
{

}

void GameBackend::update(double dt)
{
    
}

void GameBackend::run()
{
    /*
    PhysicsManager &physManager = ServiceLocator::getPhysicsManager();
    GameServer &networkManager = ServiceLocator::getGameServer();

    m_shouldRun = true;

    do
    {
        m_doUpdate.wait(false);
        physManager.update(m_deltaTime);
        networkManager.update(m_deltaTime);
    } while(m_shouldRun);
    */
}

void GameBackend::stop()
{
    m_shouldRun = false;
}
