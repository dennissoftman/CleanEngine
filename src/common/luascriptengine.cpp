#include <fstream>
#include <functional>
#include <memory>
#include <format>
#include <future>
#include <spdlog/spdlog.h>

#include "common/luascriptengine.hpp"
#include "common/servicelocator.hpp"

#include "client/gamefrontend.hpp"

// entities
#include "common/entities/camera3d.hpp"
SOL_BASE_CLASSES(Camera3D, Entity);

SOL_DERIVED_CLASSES(Entity, Camera3D);

// components
#include "common/entities/staticmesh.hpp"
SOL_BASE_CLASSES(StaticMesh, Component);

#include "common/entities/bodycomponent.hpp"
SOL_BASE_CLASSES(BodyComponent, Component);

#include "common/entities/networksynccomponent.hpp"
SOL_BASE_CLASSES(NetworkSyncComponent, Component);

SOL_DERIVED_CLASSES(Component, StaticMesh, BodyComponent, NetworkSyncComponent);
//

// ui
#include "client/ui/uilabel.hpp"
SOL_BASE_CLASSES(UILabel, UIElement);

#include "client/ui/uibutton.hpp"
SOL_BASE_CLASSES(UIButton, UIElement);

#include "client/ui/uispinbox.hpp"
SOL_BASE_CLASSES(UISpinBox, UIElement);

#include "client/ui/uitextinput.hpp"
SOL_BASE_CLASSES(UITextInput, UIElement);

SOL_DERIVED_CLASSES(UIElement, UILabel, UIButton, UISpinBox, UITextInput);
//

// physics
#include "server/physicsmanager.hpp"
//

static const char *MODULE_NAME = "LuaScriptEngine";

ScriptEngine *ScriptEngine::create()
{
    return new LuaScriptEngine();
}

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
                                 sol::lib::coroutine,
                                 sol::lib::string,
#ifndef NDEBUG
                                 sol::lib::debug,
#endif
                                 sol::lib::math,
                                 sol::lib::table);

    m_globalState.add_package_loader(luaRootLoader);

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

        m_globalState.create_named_table("SceneManager",
                                         "getActiveScene", LuaScriptEngine::SceneManager_getActiveScene);

        m_globalState.create_named_table("Input",
                                         "onKeyboard", LuaScriptEngine::EventManager_onKeyEvent,
                                         "onMouseButton", LuaScriptEngine::EventManager_onMouseButtonEvent,
                                         "onMouseMove", LuaScriptEngine::EventManager_onMouseMoveEvent,
                                         "onMouseScroll", LuaScriptEngine::EventManager_onMouseScrollEvent);

        m_globalState.create_named_table("UI",
                                         "addElement", LuaScriptEngine::UI_addElement);

        m_globalState.create_named_table("AudioManager",
                                         "loadSound", LuaScriptEngine::AudioManager_loadSound,
                                         "playSound", LuaScriptEngine::AudioManager_playSound,
                                         "loadMusic", LuaScriptEngine::AudioManager_loadMusic,
                                         "playMusic", LuaScriptEngine::AudioManager_playMusic);

        m_globalState.create_named_table("Physics",
                                         "explode", LuaScriptEngine::Physics_explode);

        m_globalState.create_named_table("ResourceManager",
                                         "getFile", LuaScriptEngine::ResourceManager_getFile);

        m_globalState.create_named_table("NetworkManager",
                                         "host", LuaScriptEngine::NetworkManager_host,
                                         "connect", LuaScriptEngine::NetworkManager_connect);
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
                                                  sol::meta_function::length, [](const glm::vec3 &v) { return glm::length(v); },
                                                  sol::meta_function::multiplication, mul_overloads,
                                                  sol::meta_function::to_string, [](const glm::vec2 &v) { return std::format("vec2({}; {})", v.x, v.y); } );
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
                                                  sol::meta_function::length, [](const glm::vec3 &v) { return glm::length(v); },
                                                  sol::meta_function::multiplication, mul_overloads,
                                                  sol::meta_function::to_string, [](const glm::vec3 &v) { return std::format("vec3({}; {}; {})", v.x, v.y, v.z); });
        }

        { // quaternion
            auto mul_overloads = sol::overload(
                        [](const glm::quat &q1, const glm::quat &q2) -> glm::quat { return q1 * q2; },
                        [](const glm::quat &q, const glm::vec3 &v) -> glm::vec3 { return q * v; }
            );

            m_globalState.new_usertype<glm::quat>("quat",
                                                  sol::call_constructor,
                                                  sol::factories([&]() { return glm::quat(); },
                                                                 [&](const glm::vec3 &angle) { return glm::quat(angle); },
                                                                 [&](float x, float y, float z, float w) { return glm::quat(w, x, y, z); } ),
                                                  "x", &glm::quat::x,
                                                  "y", &glm::quat::y,
                                                  "z", &glm::quat::z,
                                                  "w", &glm::quat::w,
                                                  sol::meta_function::multiplication, mul_overloads,
                                                  sol::meta_function::to_string, [](const glm::quat &v) { return std::format("quat({}; {}; {}; {})", v.x, v.y, v.z, v.w); });
        }
    }

    m_globalState.new_usertype<uuids::uuid>("UUID",
                                            sol::call_constructor,
                                            sol::factories([&](const std::string &val) { return uuids::uuid::from_string(val); }));

    m_globalState.new_usertype<Scene3D>("Scene3D",
                                        "addObject", &Scene3D::addObject,
                                        "getObject", &Scene3D::getObject,
                                        "getCamera", &Scene3D::getCamera,
                                        "setLightCount", &Scene3D::setLightCount,
                                        "setLightPosition", &Scene3D::setLightPosition,
                                        "setLightColor", &Scene3D::setLightColor);

    // defining object types
    m_globalState.new_usertype<Entity>("Entity",
                                       sol::call_constructor,
                                       sol::factories([&]() { return std::make_shared<Entity>(); },
                                                      [&](const uuids::uuid& id) { return std::make_shared<Entity>(id); } ),
                                       "getID", &Entity::getID,
                                       "setPosition", &Entity::setPosition,
                                       "getPosition", &Entity::getPosition,
                                       "setRotation", &Entity::setRotation,
                                       "setEulerRotation", &Entity::setEulerRotation,
                                       "getRotation", &Entity::getRotation,
                                       "getEulerRotation", &Entity::getEulerRotation,
                                       "setScale", &Entity::setScale,
                                       "getScale", &Entity::getScale,
                                       "onUpdate", &Entity::updateSubscribe,
                                       "destroy", &Entity::destroy,
                                       "onDestroy", &Entity::destroySubscribe,
                                       "attachComponent", &Entity::attachComponent,
                                       "removeComponent", &Entity::removeComponent);

    m_globalState.new_usertype<Camera3D>("Camera3D",
                                         sol::call_constructor,
                                         sol::factories([&]() { return std::make_shared<Camera3D>(); }),
                                         "setPosition", &Camera3D::setPosition,
                                         "getPosition", &Camera3D::getPosition,
                                         "setRotation", &Camera3D::setRotation,
                                         "setEulerRotation", &Camera3D::setEulerRotation,
                                         "getRotation", &Camera3D::getRotation,
                                         "getEulerRotation", &Camera3D::getEulerRotation,
                                         "front", &Camera3D::frontVector,
                                         "right", &Camera3D::rightVector,
                                         "up", &Camera3D::upVector,
                                         "onUpdate", &Camera3D::updateSubscribe);

    // components
    m_globalState.new_usertype<NetworkSyncComponent>("NetworkSyncComponent",
                                                     sol::call_constructor,
                                                     sol::factories([&](std::shared_ptr<Entity> parent) { return std::make_shared<NetworkSyncComponent>(parent); }));

    m_globalState.new_usertype<UILabel>("Label",
                                        sol::call_constructor,
                                        sol::factories([&]() -> std::shared_ptr<UILabel> { return std::make_shared<UILabel>(); }),
                                        "setText", &UILabel::setText,
                                        "getText", &UILabel::text);

    m_globalState.new_usertype<UIButton>("Button",
                                         sol::call_constructor,
                                         sol::factories([&]() -> std::shared_ptr<UIButton> { return std::make_shared<UIButton>(); }),
                                         "setText", &UIButton::setText,
                                         "getText", &UIButton::text,
                                         "onClick", &UIButton::clickSubscribe);

    m_globalState.new_usertype<UISpinBox>("SpinBox",
                                          sol::call_constructor,
                                          sol::factories([&]() -> std::shared_ptr<UISpinBox> { return std::make_shared<UISpinBox>(); }),
                                          "setLabel", &UISpinBox::setLabel,
                                          "getLabel", &UISpinBox::getLabel,
                                          "setMinimum", &UISpinBox::setMinimum,
                                          "getMinimum", &UISpinBox::getMinimum,
                                          "setMaximum", &UISpinBox::setMaximum,
                                          "getMaximum", &UISpinBox::getMaximum,
                                          "setValue", &UISpinBox::setValue,
                                          "getValue", &UISpinBox::getValue,
                                          "onChangeValue", &UISpinBox::changeValueSubscribe);

    m_globalState.new_usertype<UITextInput>("TextInput",
                                            sol::call_constructor,
                                            sol::factories([&]() -> std::shared_ptr<UITextInput> { return std::make_shared<UITextInput>(); }),
                                            "setLabel", &UITextInput::setLabel,
                                            "getLabel", &UITextInput::getLabel,
                                            "setValue", &UITextInput::setValue,
                                            "getValue", &UITextInput::getValue,
                                            "onChangeValue", &UITextInput::changeValueSubscribe);

    // other classes
    m_globalState.new_usertype<PhysicsBodyShapeInfo>("BodyShapeInfo",
                                                     sol::call_constructor,
                                                     sol::factories(
                                                         [&]() { return PhysicsBodyShapeInfo(); },
                                                         [&](float radius) { return PhysicsBodyShapeInfo(radius); },
                                                         [&](const glm::vec3 &extents) { return PhysicsBodyShapeInfo(extents); }),
                                                     "type", &PhysicsBodyShapeInfo::getShapeType);
    m_globalState.new_usertype<PhysicsBodyProperties>("BodyProperties",
                                                      sol::call_constructor,
                                                      sol::factories(
                                                          [&]() { return PhysicsBodyProperties(); },
                                                          [&](float friction) { return PhysicsBodyProperties(friction); },
                                                          [&](float friction, float restitution) { return PhysicsBodyProperties(friction, restitution); }));
    m_globalState.new_usertype<PhysicsBodyCreateInfo>("BodyCreateInfo",
                                                      sol::call_constructor,
                                                      sol::factories(
                                                        [&](const PhysicsBodyShapeInfo &shape) { return PhysicsBodyCreateInfo(shape); },
                                                        [&](const PhysicsBodyShapeInfo &shape, float mass) { return PhysicsBodyCreateInfo(shape, mass); },
                                                        [&](const PhysicsBodyShapeInfo &shape, float mass, const PhysicsBodyProperties &props) { return PhysicsBodyCreateInfo(shape, mass, props); },
                                                        [&](const PhysicsBodyShapeInfo &shape, float mass, const PhysicsBodyProperties &props, const glm::vec3 &impulse) { return PhysicsBodyCreateInfo(shape, mass, props, impulse); }),
                                                      "shape", &PhysicsBodyCreateInfo::getShapeInfo,
                                                      "mass", &PhysicsBodyCreateInfo::getMass,
                                                      "properties", &PhysicsBodyCreateInfo::getBodyProperties,
                                                      "impulse", &PhysicsBodyCreateInfo::getImpulse);

    m_globalState.new_usertype<BodyComponent>("BodyComponent",
                                              sol::call_constructor,
                                              sol::factories([&](const PhysicsBodyCreateInfo &cInfo, std::shared_ptr<Entity> parent) { return BodyComponent::createComponent(cInfo, parent); }));
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
                spdlog::warn("Init script not found");
                return;
            }
        }

        std::string scriptText(reinterpret_pointer_cast<const char[]>(scData.data).get(), scData.size);
        sol::protected_function_result result = m_globalState.safe_script(scriptText);
        if(!result.valid())
        {
            sol::error err = result;
            spdlog::error("Script execution failed: " + std::string(err.what()));
        }
    }
    catch(const std::exception &e)
    {
        spdlog::error(std::string(e.what()));
    }
    spdlog::debug("Script engine init completed");
}

int LuaScriptEngine::luaRootLoader(lua_State *L)
{
    std::string path = sol::stack::get<std::string>(L, 1); // get path argument

    if(!path.starts_with(":/") && !path.starts_with("/"))
        path = "data/scripts/" + path;

    if(!path.ends_with(".lua"))
        path += ".lua";

    DataResource scriptData = ServiceLocator::getResourceManager().getResource(path, true);
    if(scriptData.size > 0)
    {
        luaL_loadbuffer(L, reinterpret_pointer_cast<const char[]>(scriptData.data).get(), scriptData.size, path.c_str());
        return 1;
    }
    return 0;
}

void LuaScriptEngine::Debug_log(const std::string &msg)
{
    spdlog::info(msg);
}

void LuaScriptEngine::Debug_warning(const std::string &msg)
{
    spdlog::warn(msg);
}

void LuaScriptEngine::Debug_error(const std::string &msg)
{
    spdlog::error(msg);
}

void LuaScriptEngine::Client_onUpdateEvent(const std::function<void (double)> &slot)
{
    GameFrontend::corePtr->updateSubscribe(slot);
}

double LuaScriptEngine::Client_getDeltaTime()
{
    return GameFrontend::corePtr->getDeltaTime();
}

double LuaScriptEngine::Client_getElapsedTime()
{
    return GameFrontend::corePtr->getElapsedTime();
}

void LuaScriptEngine::Client_lockCursor()
{
    GameFrontend::corePtr->lockCursor();
}

void LuaScriptEngine::Client_releaseCursor()
{
    GameFrontend::corePtr->unlockCursor();
}

void LuaScriptEngine::Client_exit()
{
    GameFrontend::corePtr->terminate();
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
    return GameFrontend::corePtr->getScene();
}

std::string LuaScriptEngine::ResourceManager_getFile(const std::string &path)
{
    auto hdr = ServiceLocator::getResourceManager().getResource(path, true);
    auto data = hdr.data.get();
    return std::string(data, data+hdr.size);
}

void LuaScriptEngine::UI_addElement(std::shared_ptr<UIElement> el)
{
    ServiceLocator::getUIManager().addElement(el);
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

void LuaScriptEngine::Physics_explode(const glm::vec3 &pos, float radius, float power)
{
    ServiceLocator::getPhysicsManager().explode(pos, radius, power);
}

bool LuaScriptEngine::NetworkManager_host(uint16_t port, int32_t max_clients)
{
    return ServiceLocator::getGameServer().host(NetworkServerProperties{port, max_clients});
}

bool LuaScriptEngine::NetworkManager_connect(const std::string &ip, uint16_t port)
{
    return ServiceLocator::getGameServer().connect(NetworkClientProperties{ip, port});
}
