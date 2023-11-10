#include "server/scene3d.hpp"
#include "common/entities/entity.hpp"
#include <boost/bind.hpp>
#include <format>

Scene3D::Scene3D()
{

}

Scene3D::~Scene3D()
{
    terminate();
}

void Scene3D::draw(Renderer *rend)
{
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
    if(m_objects.find(other->getID()) == m_objects.end())
        m_objects[other->getID()] = other;
}

void Scene3D::removeObject(std::shared_ptr<Entity> other)
{
    for (auto it = m_objects.begin(); it != m_objects.end(); ++it)
    {
        if(it->second->getID() == other->getID())
        {
            // mark removed
            it->second->destroy();
            // remove from updates
            m_objects.erase(it);
            break;
        }
    }
}

std::weak_ptr<Entity> Scene3D::getObject(const uuids::uuid &id) const
{
    if(m_objects.find(id) != m_objects.end())
        return m_objects.at(id);
    return std::weak_ptr<Entity>();
}

void Scene3D::clear()
{
    m_objects.clear();
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
    
}

void Scene3D::setLightColor(const glm::vec3 &color, uint32_t id)
{
    
}

void Scene3D::setLightCount(uint32_t count)
{
    
}

size_t Scene3D::getObjectCount() const
{
    return m_objects.size();
}
