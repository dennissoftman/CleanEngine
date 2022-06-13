#include "common/luascriptengine.hpp"
#include "common/servicelocator.hpp"

#ifdef CLIENT_GLFW
#include "client/gameclientglfw.hpp"
#endif

// used classes
#include "common/entities/staticmesh.hpp"
SOL_BASE_CLASSES(StaticMesh, Entity);

#include "common/entities/camera3d.hpp"
SOL_BASE_CLASSES(Camera3D, Entity);

SOL_DERIVED_CLASSES(Entity, StaticMesh, Camera3D);
//

#include <fstream>
#include <functional>
#include <memory>

static const char *MODULE_NAME = "LuaScriptEngine";

LuaScriptEngine::LuaScriptEngine()
{

}

LuaScriptEngine::~LuaScriptEngine()
{

}

void LuaScriptEngine::init()
{
    m_globalState.open_libraries(sol::lib::base,
                                 sol::lib::package,
                                 sol::lib::string,
#ifndef NDEBUG
                                 sol::lib::debug,
#endif
                                 sol::lib::math,
                                 sol::lib::table);

    // init functions, namespaces, etc.
    {
        m_globalState.create_named_table("Debug",
                                         "log", LuaScriptEngine::Debug_log);

        m_globalState.create_named_table("Client",
                                         "onUpdate", LuaScriptEngine::Client_onUpdateEvent,
                                         "getDeltaTime", LuaScriptEngine::Client_getDeltaTime,
                                         "getElapsedTime", LuaScriptEngine::Client_getElapsedTime,
                                         "lockCursor", LuaScriptEngine::Client_lockCursor,
                                         "releaseCursor", LuaScriptEngine::Client_releaseCursor,
                                         "exit", LuaScriptEngine::Client_exit);

        m_globalState.create_named_table("MaterialManager",
                                         "loadImage", LuaScriptEngine::MaterialManager_loadImage,
                                         "loadColor", LuaScriptEngine::MaterialManager_loadColor);

        m_globalState.create_named_table("ModelManager",
                                         "loadModel", LuaScriptEngine::ModelManager_loadModel,
                                         "getModel", LuaScriptEngine::ModelManager_getModel);

        m_globalState.create_named_table("SceneManager",
                                         "getActiveScene", LuaScriptEngine::SceneManager_getActiveScene);

        m_globalState.create_named_table("Input",
                                         "onKeyboard", LuaScriptEngine::EventManager_onKeyEvent,
                                         "onMouseButton", LuaScriptEngine::EventManager_onMouseButtonEvent,
                                         "onMouseMove", LuaScriptEngine::EventManager_onMouseMoveEvent,
                                         "onMouseScroll", LuaScriptEngine::EventManager_onMouseScrollEvent);

        m_globalState.create_named_table("AudioManager",
                                         "loadSound", LuaScriptEngine::AudioManager_loadSound,
                                         "playSound", LuaScriptEngine::AudioManager_playSound,
                                         "loadMusic", LuaScriptEngine::AudioManager_loadMusic,
                                         "playMusic", LuaScriptEngine::AudioManager_playMusic);
    }

    // register object types
    { // vectors
        { // vec2
            auto mul_overloads = sol::overload(
                [](const glm::vec2& v1, const glm::vec2& v2) -> glm::vec2 { return v1*v2; },
                [](const glm::vec2& v1, float f) -> glm::vec2 { return v1*f; },
                [](float f, const glm::vec2& v1) -> glm::vec2 { return f*v1; }
            );

            m_globalState.new_usertype<glm::vec2>("vec2",
                                                  sol::call_constructor,
                                                  sol::factories([&](float a) {return glm::vec2(a);},
                                                                 [&](float x, float y) {return glm::vec2(x, y);}),
                                                  "x", &glm::vec2::x,
                                                  "y", &glm::vec2::y,
                                                  sol::meta_function::addition, [](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 + v2; },
                                                  sol::meta_function::subtraction, [](const glm::vec2 &v1, const glm::vec2 &v2) -> glm::vec2 { return v1 - v2; },
                                                  sol::meta_function::multiplication, mul_overloads);
        }

        { // vec3
            auto mul_overloads = sol::overload(
                [](const glm::vec3& v1, const glm::vec3& v2) -> glm::vec3 { return v1*v2; },
                [](const glm::vec3& v1, float f) -> glm::vec3 { return v1*f; },
                [](float f, const glm::vec3& v1) -> glm::vec3 { return f*v1; }
            );

            m_globalState.new_usertype<glm::vec3>("vec3",
                                                  sol::call_constructor,
                                                  sol::factories([&](float a) {return glm::vec3(a);},
                                                                 [&](float x, float y, float z) {return glm::vec3(x, y, z);}),
                                                  "x", &glm::vec3::x,
                                                  "y", &glm::vec3::y,
                                                  "z", &glm::vec3::z,
                                                  sol::meta_function::addition, [](const glm::vec3 &v1, const glm::vec3 &v2) -> glm::vec3 { return v1 + v2; },
                                                  sol::meta_function::subtraction, [](const glm::vec3 &v1, const glm::vec3 &v2) -> glm::vec3 { return v1 - v2; },
                                                  sol::meta_function::multiplication, mul_overloads);
        }

        { // quaternion
            auto mul_overloads = sol::overload(
                        [](const glm::quat &q1, const glm::quat &q2) -> glm::quat { return q1 * q2; },
                        [](const glm::quat &q, const glm::vec3 &v) -> glm::vec3 { return q * v; }
            );

            m_globalState.new_usertype<glm::quat>("quat",
                                                  sol::call_constructor,
                                                  sol::factories([&]() { return glm::quat(); },
                                                                 [&](const glm::vec3 &angle) { return glm::quat(angle); }),
                                                  "x", &glm::quat::x,
                                                  "y", &glm::quat::y,
                                                  "z", &glm::quat::z,
                                                  "w", &glm::quat::w,
                                                  sol::meta_function::multiplication, mul_overloads);
        }
    }

    m_globalState.new_usertype<Scene3D>("Scene3D",
                                        "addObject", &Scene3D::addObject,
                                        "getCamera", &Scene3D::getCamera);

    // defining object types
    m_globalState.new_usertype<Entity>("Entity",
                                       sol::no_constructor,
                                       "setPosition", &Entity::setPosition,
                                       "getPosition", &Entity::getPosition,
                                       "setRotation", &Entity::setRotation,
                                       "setEulerRotation", &Entity::setEulerRotation,
                                       "getRotation", &Entity::getRotation,
                                       "setScale", &Entity::setScale);

    m_globalState.new_usertype<Camera3D>("Camera3D",
                                         sol::factories([&]() -> std::shared_ptr<Camera3D> { return std::make_shared<Camera3D>(); }),
                                         "setPosition", &Camera3D::setPosition,
                                         "getPosition", &Camera3D::getPosition,
                                         "setRotation", &Camera3D::setRotation,
                                         "setEulerRotation", &Camera3D::setEulerRotation,
                                         "getRotation", &Camera3D::getRotation,
                                         "front", &Camera3D::frontVector,
                                         "right", &Camera3D::rightVector,
                                         "up", &Camera3D::upVector,
                                         "onUpdate", &Camera3D::updateSubscribe);

    m_globalState.new_usertype<StaticMesh>("StaticMesh",
                                           sol::factories([&]() -> std::shared_ptr<StaticMesh> { return std::make_shared<StaticMesh>(); }),
                                           "setPosition", &StaticMesh::setPosition,
                                           "getPosition", &StaticMesh::getPosition,
                                           "setRotation", &StaticMesh::setRotation,
                                           "setEulerRotation", &StaticMesh::setEulerRotation,
                                           "getRotation", &StaticMesh::getRotation,
                                           "setScale", &StaticMesh::setScale,
                                           "setModel", &StaticMesh::setModel,
                                           "onUpdate", &StaticMesh::updateSubscribe);
    //

    // move init script to config file?
    try
    {
        DataResource scData = ServiceLocator::getResourceManager().getResource("data/scripts/init.lua");
        if(scData.size == 0)
        {
            scData = ServiceLocator::getResourceManager().getResource(":/scripts/init.lua");
            if(scData.size == 0)
            {
                ServiceLocator::getLogger().warning(MODULE_NAME, "Init script not found");
                return;
            }
        }

        sol::protected_function_result result = m_globalState.safe_script(static_pointer_cast<const char>(scData.data).get());
        if(!result.valid())
        {
            sol::error err = result;
            ServiceLocator::getLogger().error(MODULE_NAME, "Script execution failed: " + std::string(err.what()));
        }
    }
    catch(const std::exception &e)
    {
        ServiceLocator::getLogger().error(MODULE_NAME, std::string(e.what()));
    }
    ServiceLocator::getLogger().info(MODULE_NAME, "Script engine init completed");
}

void LuaScriptEngine::Debug_log(const std::string &msg)
{
    ServiceLocator::getLogger().info(MODULE_NAME, msg);
}

void LuaScriptEngine::Debug_warning(const std::string &msg)
{
    ServiceLocator::getLogger().warning(MODULE_NAME, msg);
}

void LuaScriptEngine::Debug_error(const std::string &msg)
{
    ServiceLocator::getLogger().error(MODULE_NAME, msg);
}

void LuaScriptEngine::Client_onUpdateEvent(const std::function<void (double)> &slot)
{
    ServiceLocator::getEventManager().updateSubscribe(slot);
}

double LuaScriptEngine::Client_getDeltaTime()
{
#ifdef CLIENT_GLFW
    return GameClientGLFW::corePtr->getDeltaTime();
#else
    return 0;
#endif
}

double LuaScriptEngine::Client_getElapsedTime()
{
#ifdef CLIENT_GLFW
    return GameClientGLFW::corePtr->getElapsedTime();
#else
    return 0;
#endif
}

void LuaScriptEngine::Client_lockCursor()
{
#ifdef CLIENT_GLFW
    // for now
    glfwSetInputMode(GameClientGLFW::corePtr->getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
}

void LuaScriptEngine::Client_releaseCursor()
{
#ifdef CLIENT_GLFW
    // for now
    glfwSetInputMode(GameClientGLFW::corePtr->getWindowPtr(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
#endif
}

void LuaScriptEngine::Client_exit()
{
#ifdef CLIENT_GLFW
    GameClientGLFW::corePtr->terminate();
#endif
}

void LuaScriptEngine::MaterialManager_loadImage(const std::string &path, const std::string &name)
{
    try
    {
        DataResource imgData = ServiceLocator::getResourceManager().getResource(path);
        ServiceLocator::getMatManager().loadImage(imgData, name);
    }
    catch(const std::exception &e)
    {
        ServiceLocator::getLogger().error(MODULE_NAME, "Failed to load image: " + std::string(e.what()));
    }
}

void LuaScriptEngine::MaterialManager_loadColor(const glm::vec3 &color, const std::string &name)
{
    ServiceLocator::getMatManager().loadColor(glm::vec4(color, 1.f), name);
}

void LuaScriptEngine::ModelManager_loadModel(const std::string &path, const std::string &name)
{
    try
    {
        DataResource mdlData = ServiceLocator::getResourceManager().getResource(path);
        ServiceLocator::getModelManager().loadModel(mdlData.data.get(), mdlData.size,
                                                    name, path.substr(path.length()-path.find_last_of(".")).c_str());
    }
    catch(const std::exception &e)
    {
        ServiceLocator::getLogger().error(MODULE_NAME, "Failed to load model: " + std::string(e.what()));
    }
}

const Model3D *LuaScriptEngine::ModelManager_getModel(const std::string &name)
{
    return ServiceLocator::getModelManager().getModel(name);
}

void LuaScriptEngine::AudioManager_loadSound(const std::string &path, const std::string &name)
{
    ServiceLocator::getAudioManager().loadSound(path, name);
}

void LuaScriptEngine::AudioManager_playSound(const std::string &name, const glm::vec3 &pos)
{
    ServiceLocator::getAudioManager().playSound(name, SoundPropertiesInfo{1.f, 1.f, pos, glm::vec3()});
}

void LuaScriptEngine::AudioManager_loadMusic(const std::string &path, const std::string &name)
{
    ServiceLocator::getAudioManager().loadMusic(path, name);
}

void LuaScriptEngine::AudioManager_playMusic(const std::string &name, bool looped)
{
    ServiceLocator::getAudioManager().playMusic(name, MusicPropertiesInfo{1.f, 1.f, looped});
}

Scene3D &LuaScriptEngine::SceneManager_getActiveScene()
{
    return std::ref(ServiceLocator::getSceneManager().activeScene());
}

void LuaScriptEngine::EventManager_onKeyEvent(const clean::key_callback &slot)
{
    ServiceLocator::getEventManager().keyboardSubscribe(slot);
}

void LuaScriptEngine::EventManager_onMouseButtonEvent(const clean::mouse_button_callback &slot)
{
    ServiceLocator::getEventManager().mouseButtonSubscribe(slot);
}

void LuaScriptEngine::EventManager_onMouseMoveEvent(const clean::mouse_pos_callback &slot)
{
    ServiceLocator::getEventManager().mousePositionSubscribe(slot);
}

void LuaScriptEngine::EventManager_onMouseScrollEvent(const clean::mouse_scroll_callback &slot)
{
    ServiceLocator::getEventManager().mouseScrollSubscribe(slot);
}
