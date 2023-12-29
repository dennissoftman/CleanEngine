#ifndef PHYSICSMANAGER_HPP
#define PHYSICSMANAGER_HPP

#include <variant>
#include <glm/gtx/quaternion.hpp>

enum class PhysicsShape
{
    eUndefined,
    eBox,
    eSphere
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

    std::variant<glm::vec3, float> getShapeData() const
    {
        return m_data;
    }

    PhysicsShape getShapeType() const
    {
        return m_shape;
    }

private:
    PhysicsShape m_shape;
    std::variant<glm::vec3, float> m_data;
};

class PhysicsBodyProperties
{
public:
    PhysicsBodyProperties(float friction=0.8f, float restitution=0.2f)
        : m_friction(friction), m_restitution(restitution)
    {

    }
    float getFriction() const { return m_friction; }
    float getRestitution() const { return m_restitution; }
private:
    float m_friction;
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

// event data

struct PhysicsRaycastData
{
    glm::vec3 pos;
};

struct PhysicsContactData
{
    glm::vec3 pos;
    glm::vec3 velocity;
    float mass=1.f;
};

//

class Entity;
typedef void(*OnRaycastHitCallback)(Entity *obj, const PhysicsRaycastData &data);
typedef void(*OnContactBeginCallback)(Entity *obj, const PhysicsContactData &data);


class PhysicsManager
{
public:
    virtual ~PhysicsManager() {}

    static PhysicsManager *create();

    virtual void init() = 0;

    virtual void update(double dt) = 0;

    virtual void clear() = 0;

    virtual void setRaycastCallback(OnRaycastHitCallback callb) = 0;
    virtual bool raycast(const glm::vec3 &pos, const glm::vec3 &dir, float len) = 0;
    virtual void explode(const glm::vec3 &pos, float radius, float power) = 0;

    virtual void* createBody(const PhysicsBodyCreateInfo &cInfo, const glm::vec3& pos=glm::vec3(0.f), const glm::quat& rot=glm::quat(glm::vec3(0.f))) = 0;
};

#endif
