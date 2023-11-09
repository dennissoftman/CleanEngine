#ifndef SCENE3D_HPP
#define SCENE3D_HPP

#include "client/renderer.hpp"
#include "server/physicsmanager.hpp"
#include "common/entities/camera3d.hpp"

#include <uuid/uuid_v4.h>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

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

    std::weak_ptr<Entity> getObject(const UUIDv4::UUID& id) const;

    void clear();

    void setCamera(const Camera3D &cam);
    Camera3D &getCamera();

    void setLightPosition(const glm::vec3 &pos, uint32_t id=0);
    void setLightColor(const glm::vec3 &color, uint32_t id=0);
    void setLightCount(uint32_t count);

    size_t getObjectCount() const;

    static UUIDv4::UUIDGenerator<std::mt19937_64> m_uuidGenerator;
private:
    Camera3D m_camera;
    // for now
    std::unordered_map<UUIDv4::UUID, std::shared_ptr<Entity>> m_objects;
    clean::dirty_flag<LightingData> m_lightingData;
};

#endif // SCENE3D_HPP
