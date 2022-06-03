#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "client/renderer.hpp"

class Scene3D;

class Entity
{
public:
    virtual ~Entity() {}

    virtual void draw(Renderer *rend) = 0;
    virtual void update(double dt) = 0;

    virtual void setVisible(bool yes) = 0;

    virtual void destroy() = 0;

    virtual void setPosition(const glm::vec3 &pos) = 0;
    virtual void setRotation(const glm::quat &qrot) = 0;
    virtual void setEulerRotation(const glm::vec3 &rot) = 0;
    virtual void setScale(const glm::vec3 &scale) = 0;

    virtual const glm::vec3 &getPosition() const = 0;
    virtual glm::vec3 getEulerRotation() const = 0;
    virtual const glm::quat &getRotation() const = 0;
    virtual const glm::vec3 &getScale() const = 0;

    virtual const char *getType() = 0; // Entity, StaticMesh, etc.

    virtual void setScene(Scene3D *parent) = 0;
    virtual Scene3D *getParentScene() const = 0;
};

#endif // ENTITY_HPP
