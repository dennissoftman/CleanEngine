#include "common/entities/networksynccomponent.hpp"
#include "common/servicelocator.hpp"

NetworkSyncComponent::NetworkSyncComponent(std::shared_ptr<Entity> parent)
    : Component(parent), m_server(ServiceLocator::getGameServer())
{
}

void NetworkSyncComponent::draw(Renderer *rend)
{
}

void NetworkSyncComponent::update(double dt)
{
    std::vector<unsigned char> data;
    NetworkPacketContainer packet{};
    uint32_t offset = 0;
    if(m_parent->getPosition() != m_pos)
    {
        m_pos = m_parent->getPosition();
        packet.type |= NETWORK_PACKET_POSITION;
        int dataSize = sizeof(float) * 3;
        void *posData = reinterpret_cast<void*>(&m_pos.x);
        data.resize(data.size() + dataSize);
        memcpy(data.data()+offset, posData, dataSize);
        packet.size += dataSize;
        offset += dataSize;
    }

    if(m_parent->getRotation() != m_rot)
    {
        m_rot = m_parent->getRotation();
        packet.type |= NETWORK_PACKET_ROTATION;
        int dataSize = sizeof(float) * 4;
        void *rotData = reinterpret_cast<void*>(&m_rot.x);
        data.resize(data.size() + dataSize);
        memcpy(data.data()+offset, rotData, dataSize);
        packet.size += dataSize;
        offset += dataSize;
    }

    packet.data = data;
    m_server.sendPacket(packet);
}

bool NetworkSyncComponent::isDrawable() const
{
    return false;
}

const char *NetworkSyncComponent::getName() const
{
    return "NetworkSyncComponent";
}
