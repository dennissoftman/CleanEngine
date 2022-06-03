#include "server/scene3d.hpp"
#include "common/entities/entity.hpp"
#include "common/servicelocator.hpp"

static const char *MODULE_NAME = "Scene3D";

UUIDv4::UUIDGenerator<std::mt19937_64> Scene3D::m_uuidGenerator;

Scene3D::Scene3D()
{
}

Scene3D::~Scene3D()
{
    terminate();
}

void Scene3D::draw(Renderer *rend)
{
    // add dirty flag
    rend->setProjectionMatrix(m_camera.getProjectionMatrix());
    rend->setViewMatrix(m_camera.getViewMatrix());
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

size_t Scene3D::getObjectCount() const
{
    return m_objects.size();
}
