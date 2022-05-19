#include "luascriptengine.hpp"
#include "servicelocator.hpp"

const char *MODULE_NAME = "LuaScriptEngine";

LuaScriptEngine::LuaScriptEngine()
{

}

LuaScriptEngine::~LuaScriptEngine()
{

}

void LuaScriptEngine::init()
{

    m_globalState = luaL_newstate();
    luaL_openlibs(m_globalState);

    // register
    luabridge::getGlobalNamespace(m_globalState)
            .beginNamespace("Debug")
            .addFunction("log", LuaScriptEngine::print)
            .endNamespace();

    luabridge::getGlobalNamespace(m_globalState)
            .beginNamespace("MaterialManager")
            .addFunction("loadImage", LuaScriptEngine::MtLloadImage)
            .endNamespace();

    luabridge::getGlobalNamespace(m_globalState)
            .beginNamespace("ModelManager")
            .addFunction("loadModel", LuaScriptEngine::MdLloadModel)
            .addFunction("setMaterial", LuaScriptEngine::MdLsetMaterial)
            .endNamespace();


    {
        luaL_loadfile(m_globalState, ServiceLocator::getResourceManager().getEnginePath("data/scripts/init.lua").c_str());
        lua_pcall(m_globalState, 0, 0, 0);
    }
}

void LuaScriptEngine::MdLloadModel(const std::string &path, const std::string &name)
{
    ServiceLocator::getModelManager().loadModel(ServiceLocator::getResourceManager().getEnginePath(path),
                                                name);
}

void LuaScriptEngine::MdLsetMaterial(const std::string &model, const std::string &material)
{
    Material *mat = ServiceLocator::getMatManager().getMaterial(material);
    if(mat)
        ServiceLocator::getModelManager().setModelMaterial(model, mat);
}

void LuaScriptEngine::MtLloadImage(const std::string &path, const std::string &name)
{
    Material *newMat = Material::createMaterial();
    newMat->setImage(ServiceLocator::getResourceManager().getEnginePath(path),
                     "img");
    newMat->init();
    ServiceLocator::getMatManager().addMaterial(name, newMat);
}

void LuaScriptEngine::print(const std::string &s)
{
    ServiceLocator::getLogger().info(MODULE_NAME, s);
}
