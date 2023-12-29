#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include "server/scene3d.hpp"
#include <unordered_map>

class Renderer;
class SceneManager
{
public:
    SceneManager();
    ~SceneManager();

    void changeScene(const std::string &name);

    void update(double dt);
    void draw(Renderer *renderer);

    void addScene(std::shared_ptr<Scene3D> other, const std::string &name);
    std::shared_ptr<Scene3D> getScene(const std::string &name);
    std::shared_ptr<Scene3D> activeScene();
    void removeScene(const std::string &name);
private:
    std::unordered_map<std::string, std::shared_ptr<Scene3D>> m_scenes;
    std::shared_ptr<Scene3D> m_currentScene;
};

#endif // SCENEMANAGER_HPP
