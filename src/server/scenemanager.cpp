#include "server/scenemanager.hpp"

static std::shared_ptr<Scene3D> defaultScene = nullptr;

SceneManager::SceneManager()
{
    if(defaultScene == nullptr)
        defaultScene = std::make_shared<Scene3D>();
    m_currentScene = defaultScene;
}

SceneManager::~SceneManager()
{
}

void SceneManager::update(double dt)
{
    m_currentScene->update(dt);
}

void SceneManager::draw(Renderer *renderer)
{
    m_currentScene->draw(renderer);
}

void SceneManager::changeScene(const std::string &name)
{
    if(m_scenes.find(name) != m_scenes.end())
        m_currentScene = m_scenes[name];
}

void SceneManager::addScene(std::shared_ptr<Scene3D> other, const std::string &name)
{
    m_scenes[name] = other;
    if(m_currentScene == nullptr)
        m_currentScene = other;
}

std::shared_ptr<Scene3D> SceneManager::getScene(const std::string &name)
{
    if(m_scenes.find(name) == m_scenes.end())
        throw std::runtime_error("Scene not found");
    return m_scenes[name];
}

std::shared_ptr<Scene3D> SceneManager::activeScene()
{
    return m_currentScene;
}

void SceneManager::removeScene(const std::string &name)
{

}
