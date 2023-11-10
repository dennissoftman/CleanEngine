#ifndef SCENE3D_HPP
#define SCENE3D_HPP

#include "client/renderer.hpp"
#include "server/physicsmanager.hpp"
#include "common/entities/camera3d.hpp"

#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <stduuid/uuid.h>

#include "common/dirty_flag.hpp"

class Scene3D
{
public:
    Scene3D();
    ~Scene3D();

    void draw(Renderer *rend);
    void update(double dt);

    void terminate();

    void addObject(std::shared_ptr<Entity> other);
    void removeObject(std::shared_ptr<Entity> other);

    std::weak_ptr<Entity> getObject(const uuids::uuid& id) const;

    void clear();

    void setCamera(const Camera3D &cam);
    Camera3D &getCamera();

    void setLightPosition(const glm::vec3 &pos, uint32_t id=0);
    void setLightColor(const glm::vec3 &color, uint32_t id=0);
    void setLightCount(uint32_t count);

    size_t getObjectCount() const;
private:
    Camera3D m_camera;
    // for now
    std::unordered_map<uuids::uuid, std::shared_ptr<Entity>> m_objects;
};

#endif // SCENE3D_HPP
