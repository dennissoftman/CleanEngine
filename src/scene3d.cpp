#include "scene3d.hpp"
#include "servicelocator.hpp"

static const char *MODULE_NAME = "Scene3D";

UUIDv4::UUIDGenerator<std::mt19937_64> Scene3D::m_uuidGenerator;

Scene3D::Scene3D()
{
}

Scene3D::~Scene3D()
{
    for(auto &kv : m_objects)
        delete kv.second;
}

void Scene3D::draw(Renderer *rend)
{
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

size_t Scene3D::getObjectCount() const
{
    return m_objects.size();
}
