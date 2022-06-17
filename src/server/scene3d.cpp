#include "server/scene3d.hpp"
#include "common/entities/entity.hpp"
#include "common/servicelocator.hpp"

static const char *MODULE_NAME = "Scene3D";

UUIDv4::UUIDGenerator<std::mt19937_64> Scene3D::m_uuidGenerator;

Scene3D::Scene3D()
{
    m_lightingData.value().lightPositions[0] = glm::vec4(0, 1, 0, 1);
    m_lightingData.value().lightColors[0] = glm::vec4(1);
}

Scene3D::~Scene3D()
{
    terminate();
}

void Scene3D::draw(Renderer *rend)
{
    if(m_lightingData.is_dirty())
    {
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

void Scene3D::removeObject(const std::string &name)
{
    // DO NOT REMOVE OBJECT IF IT HAS RIGIDBODY CONNECTED!
    // SEGFAULT GUARANTEED
    // to be fixed
    m_objects.erase(name);
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

void Scene3D::setLightPosition(const glm::vec3 &pos, int id)
{
    m_lightingData.value().lightPositions[id%16] = glm::vec4(pos, 1.f);
    m_lightingData.set_dirty();
}

void Scene3D::setLightColor(const glm::vec3 &color, int id)
{
    m_lightingData.value().lightColors[id%16] = glm::vec4(color, 1.f);
    m_lightingData.set_dirty();
}

void Scene3D::setLightCount(int count)
{
    if(count < 0 || count >= Renderer::MaxLightSourceCount)
        throw std::runtime_error("invalid light source count");
    m_lightingData.value().lightCount = count;
    m_lightingData.set_dirty();
}

size_t Scene3D::getObjectCount() const
{
    return m_objects.size();
}
