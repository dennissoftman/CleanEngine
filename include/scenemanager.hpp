#ifndef SCENEMANAGER_HPP
#define SCENEMANAGER_HPP

#include "scene3d.hpp"
#include <unordered_map>

class SceneManager
{
public:
    SceneManager();
    ~SceneManager();

    void changeScene(const std::string &name);

    void addScene(Scene3D &&other, const std::string &name);
    Scene3D *getScene(const std::string &name);
    Scene3D &activeScene();
    void removeScene(const std::string &name);
private:
    std::unordered_map<std::string, Scene3D> m_scenes;
    Scene3D *m_currentScene;
};

#endif // SCENEMANAGER_HPP
