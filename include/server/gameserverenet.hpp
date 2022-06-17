#ifndef GAMESERVERENET_HPP
#define GAMESERVERENET_HPP

#include "server/gameserver.hpp"
#include <enet/enet.h>

class GameServerENET : public GameServer
{
public:
    GameServerENET();
    ~GameServerENET() override;

    void init(const ServerProperties &props) override;
    void run();
    void terminate() override;

    void setTickrate(int32_t r) override;
    int32_t tickrate() const override;
private:
    ENetHost *m_host;
    int32_t m_tickrate;
};

#endif // GAMESERVERENET_HPP
