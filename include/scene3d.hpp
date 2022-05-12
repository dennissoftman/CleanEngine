#ifndef SCENE3D_HPP
#define SCENE3D_HPP

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "uuid/uuid_v4.h"
#include "renderer.hpp"
#include <unordered_map>

/*
 * Class that is the base class for every object in game
 * from simple 3d box to complex animated vehicle
*/
class Scene3D;

class Entity
{
public:
    virtual ~Entity()
    {

    };

    virtual void draw(Renderer *rend) = 0;
    virtual void update(double dt) = 0;

    // tells scene that it should be destroyed
    virtual void destroy() = 0;

    virtual void setPos(const glm::vec3 &pos) = 0;
    virtual void setRotation(const glm::quat &rot) = 0;
    virtual void setRotation(const glm::vec3 &rot) = 0;
    virtual void setScale(const glm::vec3 &scl) = 0;

    virtual const glm::vec3 &getPosition() const = 0;
    virtual const glm::quat &getRotation() const = 0;
    virtual const glm::vec3 &getScale() const = 0;

    virtual void setScene(Scene3D *parent) = 0;
};

class StaticMesh : public Entity
{
public:
    StaticMesh()
        : rObj(nullptr), m_changedMatrix(1),
          m_pos(glm::vec3(0, 0, 0)), m_rot(glm::vec3(0, 0, 0)), m_scale(glm::vec3(1, 1, 1))
    {
    }

    ~StaticMesh() override
    {

    }

    void setModel(const Model3D *mdl)
    {
        rObj = mdl;
    }

    void draw(Renderer *rend) override
    {
        if(rObj == nullptr)
            return;

        if(m_changedMatrix)
            updateMatrix();

        rend->queueRenderObject(rObj, modelMatrix);
    }

    void update(double dt) override
    {
        (void)dt;
    }

    void destroy() override {}

    void updateMatrix()
    {
        modelMatrix = glm::translate(glm::mat4(1.f), m_pos) *
                      glm::toMat4(m_rot) *
                      glm::scale(glm::mat4(1.f), m_scale);

        m_changedMatrix = 0;
    }

    void setPos(const glm::vec3 &pos) override
    {
        m_pos = pos;
        m_changedMatrix |= 1;
    }

    void setRotation(const glm::quat &rot) override
    {
        m_rot = rot;
        m_changedMatrix |= 2;
    }

    void setRotation(const glm::vec3 &rot) override
    {
        setRotation(glm::quat(rot));
    }

    void setScale(const glm::vec3 &scl) override
    {
        m_scale = scl;
        m_changedMatrix |= 4;
    }

    const glm::vec3 &getPosition() const override
    {
        return m_pos;
    }

    const glm::quat &getRotation() const override
    {
        return m_rot;
    }

    const glm::vec3 &getScale() const override
    {
        return m_scale;
    }

    void setScene(Scene3D *parent) override
    {
        m_parentScene = parent;
    }
protected:
    // TMP
    const Model3D *rObj;
    //

    int m_changedMatrix;
    glm::mat4 modelMatrix;
    glm::vec3 m_pos;
    glm::quat m_rot;
    glm::vec3 m_scale;

    Scene3D *m_parentScene;
};

class Scene3D
{
public:
    Scene3D();
    ~Scene3D();

    void draw(Renderer *rend);
    void update(double dt);

    void addObject(Entity *other); // add with uuid
    void addObject(Entity *other, const std::string &name); // add with custom name

    size_t getObjectCount() const;

private:
    static UUIDv4::UUIDGenerator<std::mt19937_64> m_uuidGenerator;
    // for now
    std::unordered_map<std::string, Entity*> m_objects;
};

#endif // SCENE3D_HPP
