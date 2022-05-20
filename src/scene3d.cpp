#include "scene3d.hpp"
#include "servicelocator.hpp"

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
    rend->setProjectionMatrix(m_camera.getProjectionMatrix());
    rend->setViewMatrix(m_camera.getViewMatrix());
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

void Scene3D::addObject(Entity *other)
{
    addObject(other, m_uuidGenerator.getUUID().str());
}

void Scene3D::addObject(Entity *other, const std::string &name)
{
    assert(other != nullptr && "Null pointer exception");

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

void Scene3D::removeObject(Entity *other)
{
    // DO NOT REMOVE OBJECT IF IT HAS RIGIDBODY CONNECTED!
    // SEGFAULT GUARANTEED
    // to be fixed
    for(auto &kv : m_objects)
    {
        if(kv.second == other)
        {
            m_objects.erase(kv.first);
            delete other;
            break;
        }
    }
}

void Scene3D::clear()
{
    for (auto& kv : m_objects)
        delete kv.second;
    m_objects.clear();
}

const Entity *Scene3D::getObject(const std::string &name) const
{
    if(m_objects.find(name) != m_objects.end())
        return m_objects.at(name);
    return nullptr;
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
