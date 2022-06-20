#include "common/luascriptengine.hpp"
#include "common/servicelocator.hpp"

#include "client/gamefrontend.hpp"

// entities
#include "common/entities/staticmesh.hpp"
SOL_BASE_CLASSES(StaticMesh, Entity);

#include "common/entities/camera3d.hpp"
SOL_BASE_CLASSES(Camera3D, Entity);

SOL_DERIVED_CLASSES(Entity, StaticMesh, Camera3D);
//

// ui
#include "client/ui/uilabel.hpp"
SOL_BASE_CLASSES(UILabel, UIElement);

#include "client/ui/uibutton.hpp"
SOL_BASE_CLASSES(UIButton, UIElement);

#include "client/ui/uispinbox.hpp"
SOL_BASE_CLASSES(UISpinBox, UIElement);

SOL_DERIVED_CLASSES(UIElement, UILabel, UIButton, UISpinBox);
//

#include <fstream>
#include <functional>
#include <memory>

#include <future>

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

        m_globalState.create_named_table("UI",
                                         "addElement", LuaScriptEngine::UI_addElement);

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
                                        "getCamera", &Scene3D::getCamera,
                                        "setLightCount", &Scene3D::setLightCount,
                                        "setLightPosition", &Scene3D::setLightPosition,
                                        "setLightColor", &Scene3D::setLightColor);

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
                                         "getRotation", &Camera3D::getRotation,
                                         "setEulerRotation", &Camera3D::setEulerRotation,
                                         "getEulerRotation", &Camera3D::getEulerRotation,
                                         "front", &Camera3D::frontVector,
                                         "right", &Camera3D::rightVector,
                                         "up", &Camera3D::upVector,
                                         "onUpdate", &Camera3D::updateSubscribe);

    // material
    {
        auto diff_overloads = sol::overload(
            [&](const std::string &imgPath, const std::string &name)
               {
                   Material *mat = Material::create();
                   DataResource imgData = ServiceLocator::getResourceManager().getResource(imgPath);
                   ImageData img = ImageLoader::loadImageMemory(imgData.data.get(), imgData.size);
                   mat->setImage(img, "diffuse");
                   mat->init();
                   ServiceLocator::getMatManager().addMaterial(mat, name);
               },
            [&](const glm::vec3 &color, const std::string &name)
               {
                   Material *mat = Material::create();
                   mat->setColor(glm::vec4(color, 1.f), "diffuse");
                   mat->init();
                   ServiceLocator::getMatManager().addMaterial(mat, name);
               }
        );
        auto pbr_overloads = sol::overload(
            [&](const std::string &albedoPath, const std::string &normalPath, const std::string &roughnessPath,
                const std::string &metallicPath, const std::string &ambientPath, const std::string &name)
               {
                   Material *mat = Material::create();
                   auto imageLoader = [&](const DataResource &data) -> ImageData {
                       return ImageLoader::loadImageMemory(data.data.get(), data.size);
                   };

                   DataResource albedoData = ServiceLocator::getResourceManager().getResource(albedoPath),
                                normalData = ServiceLocator::getResourceManager().getResource(normalPath),
                                roughnessData = ServiceLocator::getResourceManager().getResource(roughnessPath),
                                metallicData = ServiceLocator::getResourceManager().getResource(metallicPath),
                                ambientData = ServiceLocator::getResourceManager().getResource(ambientPath);

                   std::map<Material::TextureType, std::future<ImageData>> futures;
                   futures.emplace(Material::TextureType::eAlbedo, std::async(std::launch::async, imageLoader, albedoData));
                   futures.emplace(Material::TextureType::eNormal, std::async(std::launch::async, imageLoader, normalData));
                   futures.emplace(Material::TextureType::eRoughness, std::async(std::launch::async, imageLoader, roughnessData));
                   futures.emplace(Material::TextureType::eMetallic, std::async(std::launch::async, imageLoader, metallicData));
                   futures.emplace(Material::TextureType::eAmbientOcclusion, std::async(std::launch::async, imageLoader, ambientData));

                   for(auto &kv : futures)
                       kv.second.wait();
//                   albedoData = imageLoader(albedoPath);
//                   normalData = imageLoader(normalPath);
//                   roughnessData = imageLoader(roughnessPath);
//                   metallicData = imageLoader(metallicPath);
//                   aoData = imageLoader(ambientPath);
                   mat->setPBR(futures[Material::TextureType::eAlbedo].get(),
                               futures[Material::TextureType::eNormal].get(),
                               futures[Material::TextureType::eRoughness].get(),
                               futures[Material::TextureType::eMetallic].get(),
                               futures[Material::TextureType::eAmbientOcclusion].get());
                   mat->init();
                   ServiceLocator::getMatManager().addMaterial(mat, name);
               },
            [&](const std::string &pbrPath, const std::string &name)
               {
                   Material *mat = Material::create();
                   DataResource pbrData = ServiceLocator::getResourceManager().getResource(pbrPath);
                   mat->setPBR(pbrData);
                   mat->init();
                   ServiceLocator::getMatManager().addMaterial(mat, name);
               }
        );

        m_globalState.create_named_table("Material",
                                         "createDiffuse", diff_overloads,
                                         "createPBR", pbr_overloads);
    }

    m_globalState.new_usertype<StaticMesh>("StaticMesh",
                                           sol::factories([&]() -> std::shared_ptr<StaticMesh> { return std::make_shared<StaticMesh>(); }),
                                           "setPosition", &StaticMesh::setPosition,
                                           "getPosition", &StaticMesh::getPosition,
                                           "setRotation", &StaticMesh::setRotation,
                                           "getRotation", &StaticMesh::getRotation,
                                           "setEulerRotation", &StaticMesh::setEulerRotation,
                                           "getEulerRotation", &StaticMesh::getEulerRotation,
                                           "setScale", &StaticMesh::setScale,
                                           "setModel", &StaticMesh::setModel,
                                           "onUpdate", &StaticMesh::updateSubscribe);

    m_globalState.new_usertype<UILabel>("Label",
                                        sol::factories([&]() -> std::shared_ptr<UILabel> { return std::make_shared<UILabel>(); }),
                                        "setText", &UILabel::setText,
                                        "getText", &UILabel::text);
    m_globalState.new_usertype<UIButton>("Button",
                                         sol::factories([&]() -> std::shared_ptr<UIButton> { return std::make_shared<UIButton>(); }),
                                         "setText", &UIButton::setText,
                                         "getText", &UIButton::text,
                                         "onClick", &UIButton::clickSubscribe);
    m_globalState.new_usertype<UISpinBox>("SpinBox",
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

        std::string scriptText(reinterpret_pointer_cast<const char[]>(scData.data).get(), scData.size);
        sol::protected_function_result result = m_globalState.safe_script(scriptText);
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
    return GameFrontend::corePtr->getScene();
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
