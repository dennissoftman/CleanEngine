#include "luascriptengine.hpp"
#include "servicelocator.hpp"

#include <fstream>
#include <functional>
#include <memory>

// used classes
#include "staticmesh.hpp"

SOL_BASE_CLASSES(StaticMesh, Entity);
SOL_DERIVED_CLASSES(Entity, StaticMesh);
//

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

        m_globalState.create_named_table("MaterialManager",
                                         "loadImage", LuaScriptEngine::MaterialManager_loadImage);

        m_globalState.create_named_table("ModelManager",
                                         "loadModel", LuaScriptEngine::ModelManager_loadModel,
                                         "getModel", LuaScriptEngine::ModelManager_getModel);

        m_globalState.create_named_table("SceneManager",
                                         "getActiveScene", LuaScriptEngine::SceneManager_getActiveScene);

        m_globalState.create_named_table("AudioManager",
                                         "loadSound", LuaScriptEngine::AudioManager_loadSound);
    }

    // register object types
    m_globalState.new_usertype<glm::vec3>("vec3",
                                          sol::call_constructor,
                                          sol::factories([&](float a) {return glm::vec3(a);},
                                                         [&](float x, float y, float z) {return glm::vec3(x, y, z);}),
                                          "x", &glm::vec3::x,
                                          "y", &glm::vec3::y,
                                          "z", &glm::vec3::z);

    m_globalState.new_usertype<Scene3D>("Scene3D",
                                        "addObject", &Scene3D::addObject);

    // defining object types
    m_globalState.new_usertype<Entity>("Entity", sol::no_constructor);

    m_globalState.new_usertype<StaticMesh>("StaticMesh",
                                           sol::factories([&]() -> std::shared_ptr<StaticMesh> { return std::make_shared<StaticMesh>(); }),
                                           "setPosition", &StaticMesh::setPosition,
                                           "getPosition", &StaticMesh::getPosition,
                                           "setRotation", &StaticMesh::setEulerRotation,
                                           "setScale", &StaticMesh::setScale,
                                           "setModel", &StaticMesh::setModel);
    //

    // move init script to config file?
    try
    {
        DataResource scData = ServiceLocator::getResourceManager().getResource("data/scripts/init.lua");

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

Scene3D &LuaScriptEngine::SceneManager_getActiveScene()
{
    return std::ref(ServiceLocator::getSceneManager().activeScene());
}
