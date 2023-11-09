#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "client/renderer.hpp"
#include "component.hpp"
#include <functional>
#include <glm/gtx/quaternion.hpp>
#include <uuid/uuid_v4.h>
#include <boost/signals2.hpp>

class Scene3D;

class Entity;

class Entity
{
public:
    Entity();
    Entity(const UUIDv4::UUID& id);
    ~Entity();

    virtual void draw(Renderer *rend);
    virtual void update(double dt);

    virtual UUIDv4::UUID getID() const;
    virtual void setVisible(bool yes);

    virtual void destroy();

    virtual void setPosition(const glm::vec3 &pos);
    virtual void setRotation(const glm::quat &qrot);
    virtual void setEulerRotation(const glm::vec3 &rot);
    virtual void setScale(const glm::vec3 &scale);

    virtual const glm::vec3 &getPosition() const;
    virtual glm::vec3 getEulerRotation() const;
    virtual const glm::quat &getRotation() const;
    virtual const glm::vec3 &getScale() const;

    virtual void setScene(Scene3D *parent);
    virtual Scene3D *getParentScene() const;

    virtual void attachComponent(std::shared_ptr<Component> comp);
    virtual void removeComponent(const char *name);

    // events
    virtual void updateSubscribe(const std::function<void(Entity*, double)> &callb);
    virtual void destroySubscribe(const std::function<void(Entity*)> &callb);

protected:
    UUIDv4::UUID m_id;
    bool m_visible;
    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;
    Scene3D *m_parentScene;
    std::unordered_map<const char*, std::shared_ptr<Component>> m_components;

    boost::signals2::signal<void(Entity*, double)> m_updateEvents;
    boost::signals2::signal<void(Entity*)> m_destroyEvents;
};

#endif // ENTITY_HPP
