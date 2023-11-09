#include "server/gameserverenet.hpp"
#include "common/servicelocator.hpp"

#include <stdexcept>
#include <enet/enet.h>

GameServer *GameServer::corePtr = nullptr;

static const char *MODULE_NAME = "GameServerENET";

GameServer *GameServer::create()
{
    return new GameServerENET();
}

GameServerENET::GameServerENET()
    : m_host(nullptr),
      m_client(nullptr),
      m_tickrate(64)
{
    GameServer::corePtr = this;
}

GameServerENET::~GameServerENET()
{
    terminate();
}

void GameServerENET::init()
{
    Logger &logger = ServiceLocator::getLogger();
    if(enet_initialize() != 0)
    {
        logger.error(MODULE_NAME, "Failed to initialize enet");
        return;
    }
}

bool GameServerENET::host(const NetworkServerProperties &props)
{
    if(m_host)
    {
        enet_host_destroy(m_host);
        m_host = nullptr;
    }

    Logger &logger = ServiceLocator::getLogger();

    ENetAddress addr{};
    addr.host = ENET_HOST_ANY;
    addr.port = props.port;
    m_host = enet_host_create(&addr, props.max_clients, 1, 0, 0);
    if(!m_host)
    {
        logger.error(MODULE_NAME, "Failed to create enet host");
        return false;
    }
    return true;
}

bool GameServerENET::connect(const NetworkClientProperties &props)
{
    if(m_client)
    {
        enet_peer_reset(m_client);
        m_client = nullptr;
    }

    if(m_host)
    {
        enet_host_destroy(m_host);
        m_host = nullptr;
    }

    Logger &logger = ServiceLocator::getLogger();

    m_host = enet_host_create(nullptr, 1, 1, 0, 0);
    if(!m_host)
    {
        logger.error(MODULE_NAME, "Failed to create enet host");
        return false;
    }

    ENetAddress addr{};
    enet_address_set_host(&addr, props.ip.c_str());
    addr.port = props.port;

    m_client = enet_host_connect(m_host, &addr, 1, 0);
    if(!m_client)
    {
        logger.error(MODULE_NAME, "Failed to connect to enet host");
        return false;
    }

    ENetEvent event;
    if(enet_host_service(m_host, &event, 5000) > 0)
    {
        if(event.type == ENET_EVENT_TYPE_CONNECT)
        {
            fprintf(stderr, "Connection OK");
            return true;
        }
    }
    else
    {
        logger.error(MODULE_NAME, "Connection to host timed out");
        enet_peer_reset(m_client);
    }
    return false;
}

void GameServerENET::update(double deltaTime)
{
    if(!m_host)
        return;

    m_lastUpdateTime += deltaTime;
    if((m_lastUpdateTime - m_lastTickTime) > (1000.0 / m_tickrate))
    {
        m_lastTickTime = m_lastUpdateTime;
        doUpdate(deltaTime);
    }
}

void GameServerENET::terminate()
{
    if(m_client)
    {
        enet_peer_reset(m_client);
        m_client = nullptr;
    }

    if(m_host)
    {
        enet_host_destroy(m_host);
        m_host = nullptr;
    }
    enet_deinitialize();
}

void GameServerENET::setTickrate(int32_t r)
{
    m_tickrate = (r > 0 ? r : 1);
}

int32_t GameServerENET::tickrate() const
{
    return m_tickrate;
}

void GameServerENET::sendPacket(const NetworkPacketContainer &packet)
{
    m_queuedPackets.push_back(packet);
}

void GameServerENET::onPacketReceived(const NetworkPacketContainer &packet)
{
    fprintf(stderr, "Received packet of type: %d, size: %u bytes", packet.type, packet.size);
}

void GameServerENET::doUpdate(double deltaTime)
{
    if(!m_host)
        return;

    // send queued packets
    for(auto it = m_queuedPackets.begin(); it != m_queuedPackets.end(); it++)
    {
        const NetworkPacketContainer &container = *it;
        uint32_t packetSize = sizeof(NetworkPacketContainer) + it->size;
        unsigned char* packetData = reinterpret_cast<unsigned char*>(malloc(packetSize));
        memcpy(packetData, &container.type, sizeof(uint8_t));
        memcpy(packetData+sizeof(uint8_t), &container.size, sizeof(uint32_t));
        memcpy(packetData+sizeof(uint8_t)+sizeof(uint32_t), container.data.data(), container.size);
        ENetPacket* pkt = enet_packet_create(it->data.data(), packetSize, ENET_PACKET_FLAG_RELIABLE); // add later support for unreliable packets
        if(m_client)
            enet_peer_send(m_client, 0, pkt);
        else
            enet_host_broadcast(m_host, 0, pkt);

        enet_packet_destroy(pkt);
    }

    // receive packets
    ENetEvent event;
    while(enet_host_service(m_host, &event, 0) > 0)
    {
        if(event.type == ENET_EVENT_TYPE_RECEIVE)
        {
            NetworkPacketContainer packet{};
            ENetPacket* pkt = event.packet;
            memcpy(&packet.type, pkt->data, sizeof(uint8_t));
            memcpy(&packet.size, pkt->data+sizeof(uint32_t), sizeof(uint32_t));
            packet.data.resize(packet.size);
            memcpy(packet.data.data(), pkt->data+sizeof(uint8_t)+sizeof(uint32_t), packet.size);
            enet_packet_destroy(pkt);
            onPacketReceived(packet);
        }
    }
}
