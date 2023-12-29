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
SOL_DERIVED_CLASSES(MeshComponent, StaticMesh);

#include "common/entities/animatedmodel.hpp"
SOL_BASE_CLASSES(AnimatedModelComponent, Component);
SOL_DERIVED_CLASSES(StaticMesh, AnimatedModelComponent);

#include "common/entities/bodycomponent.hpp"
SOL_BASE_CLASSES(BodyComponent, Component);

#include "common/entities/networksynccomponent.hpp"
SOL_BASE_CLASSES(NetworkSyncComponent, Component);

SOL_DERIVED_CLASSES(Component, StaticMesh, AnimatedModelComponent, BodyComponent, NetworkSyncComponent);
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

// game services
#include "common/gameservices.hpp"

// physics
#include "server/physicsmanager.hpp"


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
                                         "log", []<typename... Args>(const std::string& msg, Args &&...args) { spdlog::info(msg, args); },
                                         "warn", []<typename... Args>(const std::string& msg, Args &&...args) { spdlog::warn(msg, args); },
                                         "error", []<typename... Args>(const std::string& msg, Args &&...args) { spdlog::error(msg, args); });

        m_globalState.create_named_table("Client",
                                         "onUpdate", [](const std::function<void (double)> &slot) { GameFrontend::corePtr->updateSubscribe(slot); },
                                         "getDeltaTime", []() { return GameFrontend::corePtr->getDeltaTime(); },
                                         "getElapsedTime", []() { return GameFrontend::corePtr->getElapsedTime(); },
                                         "lockCursor", []() { GameFrontend::corePtr->lockCursor(); },
                                         "releaseCursor", []() { GameFrontend::corePtr->unlockCursor(); },
                                         "exit", []() { GameFrontend::corePtr->terminate(); });

        m_globalState.create_named_table("GameServices",
                                         "getFriends", []() { return ServiceLocator::getGameServices().getFriends(); });

        m_globalState.create_named_table("SceneManager",
                                         "getActiveScene", []() { return ServiceLocator::getSceneManager().activeScene(); });

        m_globalState.create_named_table("Input",
                                         "onKeyboard", [](const clean::key_callback &slot) { ServiceLocator::getEventManager().keyboardSubscribe(slot); },
                                         "onMouseButton", [](const clean::mouse_button_callback &slot) { ServiceLocator::getEventManager().mouseButtonSubscribe(slot); },
                                         "onMouseMove", [](const clean::mouse_pos_callback &slot) { ServiceLocator::getEventManager().mousePositionSubscribe(slot); },
                                         "onMouseScroll", [](const clean::mouse_scroll_callback &slot) { ServiceLocator::getEventManager().mouseScrollSubscribe(slot); });

        m_globalState.create_named_table("UI",
                                         "addElement", [](std::shared_ptr<UIElement> el) { ServiceLocator::getUIManager().addElement(el); });

        m_globalState.create_named_table("ModelManager",
                                         "loadModel", [](const std::string &path, const std::string& name) {
                                            try
                                            {
                                                ServiceLocator::getModelManager().import_model(path, name);
                                                return true;
                                            }
                                            catch(const std::exception &e)
                                            {
                                                spdlog::error("Failed to load model '{}': {}", name, e.what());
                                                return false;
                                            }
                                        },
                                        "getModelId", [](const std::string &name) {
                                            try
                                            {
                                                return ServiceLocator::getModelManager().getModelId(name);
                                            }
                                            catch(const std::exception &e)
                                            {
                                                spdlog::error("Failed to get model id for '{}': {}", name, e.what());
                                                return (size_t)-1;
                                            }
                                        },
                                        "getAnimation", [](const std::string& modelName, const std::string& animName) {
                                            try
                                            {
                                                return ServiceLocator::getModelManager().getAnimation(modelName, animName);
                                            }
                                            catch (const std::exception& e)
                                            {
                                                spdlog::error("Failed to get animation '{}' for model '{}'", animName, modelName);
                                                return std::shared_ptr<AnimationPrimitive>(nullptr);
                                            }
                                        });

        m_globalState.create_named_table("MaterialManager",
                                         "createMaterial", sol::factories([&](const glm::vec3 &color, const std::string &name) { return ServiceLocator::getMatManager().createMaterial(color, name); },
                                                                          [&](const std::string &path, const std::string &name) { return ServiceLocator::getMatManager().createMaterial(path, name); }));

        m_globalState.create_named_table("AudioManager",
                                         "loadSound", [](const std::string &path, const std::string& name) { return ServiceLocator::getAudioManager().loadSound(path, name); },
                                         "playSound", [](const std::string &name, const glm::vec3 &pos) { ServiceLocator::getAudioManager().playSound(name, SoundPropertiesInfo{1.f, 1.f, pos, glm::vec3()}); },
                                         "loadMusic", [](const std::string &path, const std::string& name) { return ServiceLocator::getAudioManager().loadMusic(path, name); },
                                         "playMusic", [](const std::string &name, bool looped) { ServiceLocator::getAudioManager().playMusic(name, MusicPropertiesInfo{1.f, 1.f, looped}); });

        m_globalState.create_named_table("ResourceManager",
                                         "getFile", [](const std::string& path) {
                                            std::shared_ptr<DataResource> res;
                                            try
                                            {
                                                res = ServiceLocator::getResourceManager().get(path, true);
                                            }
                                            catch(const std::exception &e)
                                            {
                                                spdlog::error(fmt::format("Failed to load file '{}': {}", path, e.what()));
                                                return std::string();
                                            }
                                            return std::string(res->data.data(), res->data.size());
                                        });

        m_globalState.create_named_table("NetworkManager",
                                         "host", [](uint16_t port, int32_t max_clients) { return ServiceLocator::getGameServer().host(NetworkServerProperties{port, max_clients}); },
                                         "connect", [](const std::string &ip, uint16_t port) { return ServiceLocator::getGameServer().connect(NetworkClientProperties{ip, port}); });
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

    m_globalState.new_usertype<FriendDetails>("FriendDetails",
                                              sol::constructors<FriendDetails()>(),
                                              "name", &FriendDetails::name,
                                              "isOnline", &FriendDetails::isOnline);

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

    m_globalState.new_usertype<StaticMesh>("StaticMeshComponent",
                                           sol::call_constructor,
                                           sol::factories([&](std::shared_ptr<Entity> parent) { return StaticMesh::createComponent(parent); }),
                                           "setModelId", &StaticMesh::setModelId);

    m_globalState.new_usertype<AnimatedModelComponent>("AnimatedModelComponent",
                                                       sol::call_constructor,
                                                       sol::factories([&](std::shared_ptr<Entity> parent) { return std::make_shared<AnimatedModelComponent>(parent); }),
                                                       "setModelId", &AnimatedModelComponent::setModelId,
                                                       "setAnimation", &AnimatedModelComponent::setAnimation,
                                                       "trigger", &AnimatedModelComponent::trigger);

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
        auto scData = ServiceLocator::getResourceManager().get("scripts/init.lua");

        std::string scriptText(scData->data.data(), scData->data.size());
        sol::protected_function_result result = m_globalState.safe_script(scriptText);
        if(!result.valid())
        {
            sol::error err = result;
            spdlog::error("Script execution failed: " + std::string(err.what()));
        }
    }
    catch(const std::exception &e)
    {
        spdlog::error(fmt::format("Failed to load init script: {}", e.what()));
        return;
    }
    spdlog::debug("Script engine init completed");
}

int LuaScriptEngine::luaRootLoader(lua_State *L)
{
    std::string path = sol::stack::get<std::string>(L, 1); // get path argument

    if(!path.ends_with(".lua"))
        path += ".lua";

    try
    {
        auto res = ServiceLocator::getResourceManager().get(path, true);
        luaL_loadbuffer(L, res->data.data(), res->data.size(), path.c_str());
        return 1;
    }
    catch(const std::exception& e)
    {
        spdlog::error(fmt::format("Failed to load script '{}': {}", path, e.what()));
        return 0;
    }
    return 0;
}
