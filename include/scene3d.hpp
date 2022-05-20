#ifndef SCENE3D_HPP
#define SCENE3D_HPP

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "uuid/uuid_v4.h"
#include "renderer.hpp"
#include "physicsmanager.hpp"
#include "camera3d.hpp"

#include <unordered_map>

/*
 * Class that is the base class for every object in game
 * from simple 3d box to complex animated vehicle
*/
class Scene3D;

class Entity
{
public:
    virtual ~Entity() { }

    virtual void draw(Renderer *rend) = 0;
    virtual void update(double dt) = 0;

    virtual void setVisible(bool yes) = 0;

    // events
    virtual void setOnContactBeginCallback(OnContactBeginCallback callb) = 0;
    virtual void OnContactBegin(const PhysicsContactData &data) = 0;

    // tells scene that it should be destroyed
    virtual void destroy() = 0;

    virtual void setPosition(const glm::vec3 &pos) = 0;
    virtual void setRotation(const glm::quat &rot) = 0;
    virtual void setRotation(const glm::vec3 &rot) = 0;
    virtual void setScale(const glm::vec3 &scl) = 0;

    virtual const glm::vec3 &getPosition() const = 0;
    virtual const glm::quat &getRotation() const = 0;
    virtual const glm::vec3 &getScale() const = 0;

    virtual void setScene(Scene3D *parent) = 0;
    virtual Scene3D *getParentScene() const = 0;
};

class Scene3D
{
public:
    Scene3D();
    ~Scene3D();

    void draw(Renderer *rend);
    void update(double dt);

    void terminate();

    void addObject(Entity *other); // add with uuid
    void addObject(Entity *other, const std::string &name); // add with custom name

    void removeObject(Entity *other);

    void clear();

    const Entity *getObject(const std::string &name) const;

    void setCamera(const Camera3D &cam);
    Camera3D &getCamera();

    size_t getObjectCount() const;

private:
    static UUIDv4::UUIDGenerator<std::mt19937_64> m_uuidGenerator;
    Camera3D m_camera;
    // for now
    std::unordered_map<std::string, Entity*> m_objects;
};

#endif // SCENE3D_HPP
