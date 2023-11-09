#ifndef NETWORKSYNCCOMPONENT_HPP
#define NETWORKSYNCCOMPONENT_HPP

#include "common/entities/component.hpp"
#include "server/gameserver.hpp"
#include <glm/gtx/quaternion.hpp>

class NetworkSyncComponent : public Component
{
public:
    NetworkSyncComponent(std::shared_ptr<Entity> parent);

    void draw(Renderer *rend);
    void update(double dt);
    bool isDrawable() const;
    const char *getName() const;

protected:
    GameServer& m_server;
    // cached values
    glm::vec3 m_pos;
    glm::quat m_rot;
};

#endif // NETWORKSYNCCOMPONENT_HPP
