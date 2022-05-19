#ifndef PHYSICSMANAGER_HPP
#define PHYSICSMANAGER_HPP

#include <variant>
#include <glm/glm.hpp>

#include "modelmanager.hpp"

enum class PhysicsShape
{
    eUndefined,
    eBox,
    eSphere,
    eTriangleMesh
};

class PhysicsBodyShapeInfo
{
public:
    PhysicsBodyShapeInfo()
        : m_shape(PhysicsShape::eUndefined)
    { }

    PhysicsBodyShapeInfo(const glm::vec3 &boxExtents)
        : m_shape(PhysicsShape::eBox)
    {
        m_data = boxExtents;
    }

    PhysicsBodyShapeInfo(float radius)
        : m_shape(PhysicsShape::eSphere)
    {
        m_data = radius;
    }

    PhysicsBodyShapeInfo(const Mesh3D *mesh)
        : m_shape(PhysicsShape::eTriangleMesh)
    {
        m_data = mesh;
    }

    std::variant<glm::vec3, float, const Mesh3D*> getShapeData() const
    {
        return m_data;
    }

    PhysicsShape getShapeType() const
    {
        return m_shape;
    }

private:
    PhysicsShape m_shape;
    std::variant<glm::vec3, float, const Mesh3D*> m_data;
};

class PhysicsBodyProperties
{
public:
    PhysicsBodyProperties(float restitution={})
        : m_restitution(restitution)
    {

    }
    float getRestitution() const { return m_restitution; }
private:
    float m_restitution;
};

class PhysicsBodyCreateInfo
{
public:
    PhysicsBodyCreateInfo(const PhysicsBodyShapeInfo &sInfo,
                          float mass={},
                          const PhysicsBodyProperties &bProps={},
                          const glm::vec3 &startImpulse={})
        : m_shapeInfo(sInfo), m_mass(mass), m_bodyProperties(bProps), m_startImpulse(startImpulse)
    {

    }

    const PhysicsBodyShapeInfo &getShapeInfo() const { return m_shapeInfo; }
    float getMass() const { return m_mass; }
    const PhysicsBodyProperties &getBodyProperties() const { return m_bodyProperties; }
    const glm::vec3 &getImpulse() const { return m_startImpulse; }

private:
    PhysicsBodyShapeInfo m_shapeInfo;
    float m_mass;
    PhysicsBodyProperties m_bodyProperties;
    glm::vec3 m_startImpulse;
};

class Entity;

class PhysicsManager
{
public:
    virtual ~PhysicsManager() {}

    virtual void init() = 0;

    virtual void update(double dt) = 0;

    virtual void createBody(const PhysicsBodyCreateInfo &cInfo, Entity *parent=nullptr) = 0;
};

#endif
