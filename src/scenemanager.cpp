#include "scenemanager.hpp"

static Scene3D *defaultScene = nullptr;

SceneManager::SceneManager()
{
    if(defaultScene == nullptr)
        defaultScene = new Scene3D();
    m_currentScene = defaultScene;
}

SceneManager::~SceneManager()
{
    delete defaultScene;
    defaultScene = nullptr;
}

void SceneManager::changeScene(const std::string &name)
{
    if(m_scenes.find(name) != m_scenes.end())
        m_currentScene = &m_scenes[name];
}

void SceneManager::addScene(Scene3D &&other, const std::string &name)
{
    m_scenes[name]= std::move(other);
    if(m_currentScene == nullptr)
        m_currentScene = &m_scenes[name];
}

Scene3D *SceneManager::getScene(const std::string &name)
{
    if(m_scenes.find(name) == m_scenes.end())
        throw std::runtime_error("Scene not found");
    return &m_scenes[name];
}

Scene3D &SceneManager::activeScene()
{
    return *m_currentScene;
}

void SceneManager::removeScene(const std::string &name)
{

}
