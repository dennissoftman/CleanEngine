#ifndef GAMESERVER_HPP
#define GAMESERVER_HPP

#include <string>
#include <cstdint>
#include <vector>
#include <memory>

class Entity;

struct NetworkServerProperties
{
    uint16_t port;
    int32_t max_clients=32;
};

struct NetworkClientProperties
{
    std::string ip;
    uint16_t port;
};

/*
 * Single network packet can contain 1 up to 6 types
 * if packet contains for example position, rotation
 * and integer value, then values are packed sequentially
 * in order they are defined in NetworkPacketType struct
 * @size = sizeof(integer) + sizeof(vec3) + sizeof(quat)
 * @data -> integer ... position ... rotation
*/

#define NETWORK_PACKET_INVALID 0x0
#define NETWORK_PACKET_INTEGER 0x1
#define NETWORK_PACKET_FLOAT 0x2
#define NETWORK_PACKET_DOUBLE 0x4
#define NETWORK_PACKET_POSITION 0x8
#define NETWORK_PACKET_ROTATION 0x10
#define NETWORK_PACKET_BLOB 0x20
#define NETWORK_PACKET_RPC 0x40

struct NetworkPacketContainer {
    uint8_t type;
    uint32_t size;
    std::vector<unsigned char> data;
};

class GameServer
{
public:
    static GameServer *create();
    virtual ~GameServer() {}

    virtual void init() = 0;
    virtual void update(double deltaTime) = 0;
    virtual bool host(const NetworkServerProperties &props) = 0;
    virtual bool connect(const NetworkClientProperties &props) = 0;
    virtual void terminate() = 0;

    virtual void setTickrate(int32_t r) = 0;
    virtual int32_t tickrate() const = 0;

    virtual void sendPacket(const NetworkPacketContainer& packet) = 0;
    virtual void onPacketReceived(const NetworkPacketContainer& packet) = 0;

    static GameServer *corePtr;
};

#endif // GAMESERVER_HPP
