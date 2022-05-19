#include "scenemanager.hpp"

SceneManager::SceneManager()
{

}

void SceneManager::addScene(Scene3D &&other, const std::string &name)
{
    m_scenes[name]= std::move(other);
}

Scene3D *SceneManager::getScene(const std::string &name)
{
    if(m_scenes.find(name) == m_scenes.end())
        throw std::runtime_error("Scene not found");
    return &m_scenes[name];
}

void SceneManager::removeScene(const std::string &name)
{

}
