#include "common/entities/staticmesh.hpp"
#include "client/renderer.hpp"
#include "common/entities/entity.hpp"
#include "common/utils.hpp"

#include <memory>
#include <glm/gtx/quaternion.hpp>

StaticMesh::StaticMesh(std::shared_ptr<Entity> parent)
    : Component(parent), rObj(nullptr)
{
    m_name = "StaticMesh_" + Utils::uuidGenerator.getUUID().str();
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
    rend->queueRenderObject(rObj, m_modelMatrix);
}

void StaticMesh::update(double dt)
{
    const glm::vec3& pos = m_parent.get()->getPosition();
    const glm::quat& rot = m_parent.get()->getRotation();
    const glm::vec3& scale = m_parent.get()->getScale();

    m_modelMatrix = glm::translate(glm::mat4(1.f), pos) *
                    glm::toMat4(rot) *
                    glm::scale(glm::mat4(1.f), scale);
}

bool StaticMesh::isDrawable() const
{
    return (rObj != nullptr);
}

const char *StaticMesh::getName() const
{
    return m_name.c_str();
}
