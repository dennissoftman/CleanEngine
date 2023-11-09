#ifndef GAMESERVERENET_HPP
#define GAMESERVERENET_HPP

#include <enet/enet.h>
#include <unordered_map>
#include "server/gameserver.hpp"

class GameServerENET : public GameServer
{
public:
    GameServerENET();
    ~GameServerENET() override;

    void init() override;
    bool host(const NetworkServerProperties &props) override;
    bool connect(const NetworkClientProperties &props) override;
    void update(double deltaTime) override;
    void terminate() override;

    void setTickrate(int32_t r) override;
    int32_t tickrate() const override;

    void sendPacket(const NetworkPacketContainer &packet) override;
    void onPacketReceived(const NetworkPacketContainer &packet) override;
private:
    void doUpdate(double deltaTime);

    std::vector<NetworkPacketContainer> m_queuedPackets;

    ENetHost* m_host;
    ENetPeer* m_client;
    int32_t m_tickrate;
    double m_lastUpdateTime, m_lastTickTime;
};

#endif // GAMESERVERENET_HPP
