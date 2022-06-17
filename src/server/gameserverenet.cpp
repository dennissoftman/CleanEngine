#include "server/gameserverenet.hpp"
#include <stdexcept>
#include <enet/enet.h>

GameServer *GameServer::corePtr = nullptr;

GameServerENET::GameServerENET()
    : m_host(nullptr),
      m_tickrate(128)
{
    GameServer::corePtr = this;
}

GameServerENET::~GameServerENET()
{
    terminate();
}

void GameServerENET::init(const ServerProperties &props)
{
    ENetAddress addr {};
    enet_address_set_host_ip(&addr, props.ip.c_str());
    addr.port = props.port;
    m_host = enet_host_create(&addr, props.max_clients, 1, 0, 0);
    if(m_host == nullptr)
    {
        throw std::runtime_error("host create failed");
    }
}

void GameServerENET::run()
{

}

void GameServerENET::terminate()
{
    if(m_host)
    {
        enet_host_destroy(m_host);
        m_host = nullptr;
    }
}

void GameServerENET::setTickrate(int32_t r)
{
    m_tickrate = (r > 0 ? r : 1);
}

int32_t GameServerENET::tickrate() const
{
    return m_tickrate;
}
