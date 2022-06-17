#ifndef GAMESERVER_HPP
#define GAMESERVER_HPP

#include <string>
#include <cstdint>

struct ServerProperties
{
    std::string ip;
    uint16_t port;
    int32_t max_clients=32;
    bool local=false;
};

class GameServer
{
public:
    virtual ~GameServer() {}

    virtual void init(const ServerProperties &props) = 0;
    virtual void terminate() = 0;

    virtual void setTickrate(int32_t r) = 0;
    virtual int32_t tickrate() const = 0;

    static GameServer *corePtr;
};

#endif // GAMESERVER_HPP
