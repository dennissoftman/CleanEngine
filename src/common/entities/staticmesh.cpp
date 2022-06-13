#include "common/entities/staticmesh.hpp"
#include "common/servicelocator.hpp"
#include <memory>

StaticMesh::StaticMesh()
    : rObj(nullptr),
      m_visible(true),
      m_changedMatrix(1),
      m_pos(glm::vec3(0, 0, 0)), m_rot(glm::vec3(0, 0, 0)), m_scale(glm::vec3(1, 1, 1))
{

}

StaticMesh::~StaticMesh()
{

}

void StaticMesh::setModel(const Model3D *mdl)
{
    rObj = mdl;
}

void StaticMesh::draw(Renderer *rend)
{
    if(!m_visible)
        return;

    if(rObj == nullptr)
        return;

    if(m_changedMatrix)
        updateMatrix();

    rend->queueRenderObject(rObj, modelMatrix);
}

void StaticMesh::update(double dt)
{
    m_updateEvents(this, dt);
}

void StaticMesh::setVisible(bool yes)
{
    m_visible = yes;
}

void StaticMesh::destroy()
{
    m_destroyEvents(this);
}

void StaticMesh::updateMatrix()
{
    modelMatrix = glm::translate(glm::mat4(1.f), m_pos) *
            glm::toMat4(m_rot) *
            glm::scale(glm::mat4(1.f), m_scale);

    m_changedMatrix = 0;
}

void StaticMesh::setPosition(const glm::vec3 &pos)
{
    m_pos = pos;
    m_changedMatrix |= 1;
}

void StaticMesh::setRotation(const glm::quat &rot)
{
    m_rot = rot;
    m_changedMatrix |= 2;
}

void StaticMesh::setEulerRotation(const glm::vec3 &rot)
{
    setRotation(glm::quat(rot));
}

void StaticMesh::setScale(const glm::vec3 &scl)
{
    m_scale = scl;
    m_changedMatrix |= 4;
}

const glm::vec3 &StaticMesh::getPosition() const
{
    return m_pos;
}

glm::vec3 StaticMesh::getEulerRotation() const
{
    return glm::eulerAngles(m_rot);
}

const glm::quat &StaticMesh::getRotation() const
{
    return m_rot;
}

const glm::vec3 &StaticMesh::getScale() const
{
    return m_scale;
}

void StaticMesh::setScene(Scene3D *parent)
{
    m_parentScene = parent;
}

Scene3D *StaticMesh::getParentScene() const
{
    return m_parentScene;
}

const char *StaticMesh::getType()
{
    return "StaticMesh";
}

void StaticMesh::updateSubscribe(const std::function<void (Entity*, double)> &callb)
{
    m_updateEvents.connect(callb);
}

void StaticMesh::destroySubscribe(const std::function<void (Entity*)> &callb)
{
    m_destroyEvents.connect(callb);
}
