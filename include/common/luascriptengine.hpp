#ifndef LUASCRIPTENGINE_HPP
#define LUASCRIPTENGINE_HPP

#include "common/scriptengine.hpp"

#define SOL_ALL_SAFETIES_ON 1

#include <limits>
#include <sol/sol.hpp>
#include <glm/vec3.hpp>

#include "common/eventmanager.hpp"

class UIElement;
class Model3D;
class Scene3D;

class LuaScriptEngine : public ScriptEngine
{
public:
    LuaScriptEngine();
    ~LuaScriptEngine();

    void init() override;

    // Logging
    static void Debug_log(const std::string &msg);
    static void Debug_warning(const std::string &msg);
    static void Debug_error(const std::string &msg);

    // Client
    static void Client_onUpdateEvent(const std::function<void(double)> &slot);
    static double Client_getDeltaTime();
    static double Client_getElapsedTime();
    static void Client_lockCursor();
    static void Client_releaseCursor();
    static void Client_exit();

    // Material manager
    static void MaterialManager_loadImage(const std::string &path, const std::string &name);
    static void MaterialManager_loadColor(const glm::vec3 &color, const std::string &name);

    // Model manager
    static void ModelManager_loadModel(const std::string &path, const std::string &name);
    static const Model3D *ModelManager_getModel(const std::string &name);

    // Audio manager
    static void AudioManager_loadSound(const std::string &path, const std::string &name);
    static void AudioManager_playSound(const std::string &name, const glm::vec3 &pos);
    static void AudioManager_loadMusic(const std::string &path, const std::string &name);
    static void AudioManager_playMusic(const std::string &name, bool looped);

    // Scene manager
    static Scene3D &SceneManager_getActiveScene();

    // UI
    static void UI_addElement(std::shared_ptr<UIElement> el);

    // Event manager
    static void EventManager_onKeyEvent(const clean::key_callback &slot);
    static void EventManager_onMouseButtonEvent(const clean::mouse_button_callback &slot);
    static void EventManager_onMouseMoveEvent(const clean::mouse_pos_callback &slot);
    static void EventManager_onMouseScrollEvent(const clean::mouse_scroll_callback &slot);
private:
    // TODO: multiple states
    sol::state m_globalState;
};

#endif // LUASCRIPTENGINE_HPP
