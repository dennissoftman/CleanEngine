#include "server/scene3d.hpp"
#include "common/entities/entity.hpp"
#include "common/servicelocator.hpp"

static const char *MODULE_NAME = "Scene3D";

UUIDv4::UUIDGenerator<std::mt19937_64> Scene3D::m_uuidGenerator;

Scene3D::Scene3D()
{
    m_lightingData.value().lightCount = 1;
    m_lightingData.value().lightPositions[0] = glm::vec4(-1.f, -1.f, -1.f, 1.f);
    m_lightingData.value().lightColors[0] = glm::vec4(1.f);
    m_lightingData.set_dirty();
}

Scene3D::~Scene3D()
{
    terminate();
}

void Scene3D::draw(Renderer *rend)
{
    if(m_lightingData.is_dirty())
    {
        rend->updateLightCount(m_lightingData.value().lightCount);
        for(int i=0; i < m_lightingData.value().lightCount; i++)
        {
            rend->updateLightPosition(m_lightingData.value().lightPositions[i], i);
            rend->updateLightColor(m_lightingData.value().lightColors[i], i);
        }
        m_lightingData.clear_dirty();
    }
    rend->updateCameraData(m_camera);
    //
    for(auto &kv : m_objects)
    {
        kv.second->draw(rend);
    }
}

void Scene3D::update(double dt)
{
    for(auto &kv : m_objects)
    {
        kv.second->update(dt);
    }
}

void Scene3D::terminate()
{
    clear();
}

void Scene3D::addObject(std::shared_ptr<Entity> other)
{
    addNamedObject(other, m_uuidGenerator.getUUID().str());
}

void Scene3D::addNamedObject(std::shared_ptr<Entity> other, const std::string &name)
{
    if(m_objects.find(name) == m_objects.end())
    {
        other->setScene(this);
        m_objects[name] = other;
    }
    else
    {
        ServiceLocator::getLogger().error(MODULE_NAME, "Object already exists in scene");
    }
}

void Scene3D::clear()
{
    m_objects.clear();
}

std::weak_ptr<Entity> Scene3D::getObject(const std::string &name) const
{
    if(m_objects.find(name) != m_objects.end())
        return m_objects.at(name);
    return std::weak_ptr<Entity>();
}

void Scene3D::setCamera(const Camera3D &cam)
{
    m_camera = cam;
}

Camera3D &Scene3D::getCamera()
{
    return m_camera;
}

void Scene3D::setLightPosition(const glm::vec3 &pos, uint32_t id)
{
    float old_w = m_lightingData.value().lightPositions[std::min(id, Renderer::MaxLightSourceCount-1)].w;
    m_lightingData.value().lightPositions[std::min(id, Renderer::MaxLightSourceCount-1)] = glm::vec4(pos, old_w);
    m_lightingData.set_dirty();
}

void Scene3D::setLightColor(const glm::vec3 &color, uint32_t id)
{
    float old_w = m_lightingData.value().lightColors[std::min(id, Renderer::MaxLightSourceCount-1)].w;
    m_lightingData.value().lightColors[std::min(id, Renderer::MaxLightSourceCount-1)] = glm::vec4(color, old_w);
    m_lightingData.set_dirty();
}

void Scene3D::setLightCount(uint32_t count)
{
    m_lightingData.value().lightCount = std::min(count, Renderer::MaxLightSourceCount);
    m_lightingData.set_dirty();
}

size_t Scene3D::getObjectCount() const
{
    return m_objects.size();
}
