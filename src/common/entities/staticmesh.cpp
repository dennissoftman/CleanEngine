#include "common/entities/staticmesh.hpp"
#include "client/renderer.hpp"
#include "common/entities/entity.hpp"
#include "common/utils.hpp"

#include <memory>
#include <glm/gtx/quaternion.hpp>

std::shared_ptr<StaticMesh> StaticMesh::createComponent(std::shared_ptr<Entity> parent)
{
    return std::make_shared<StaticMesh>(parent);
}

StaticMesh::StaticMesh(std::shared_ptr<Entity> parent)
    : MeshComponent(parent), m_meshId(-1), m_visible(true), m_castShadow(true), m_translucent(false)
{
    m_name = "StaticMesh_" + uuids::to_string(Utils::uuidGenerator());
}

StaticMesh::~StaticMesh()
{

}

void StaticMesh::draw(Renderer *rend)
{
    if(!m_visible)
        return;

    rend->queueRender(m_meshId, std::vector<size_t>{}, m_modelMatrix);
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

void StaticMesh::setModelId(uint32_t id)
{
    m_meshId = id;
}

void StaticMesh::show()
{
    m_visible = true;
}

void StaticMesh::hide()
{
    m_visible = false;
}

bool StaticMesh::isDrawable() const
{
    return m_visible;
}

void StaticMesh::castShadow(bool cast)
{
    m_castShadow = cast;
}

bool StaticMesh::isCastingShadow() const
{
    return m_castShadow;
}

void StaticMesh::setTranslucent(bool translucent)
{
    m_translucent = translucent;
}

bool StaticMesh::isTranslucent() const
{
    return m_translucent;
}

const char *StaticMesh::getName() const
{
    return m_name.c_str();
}
