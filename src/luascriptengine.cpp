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
            .beginClass<glm::vec3>("vec3")
            .addConstructor<void(*)(float)>()
            .addConstructor<void(*)(float, float, float)>()
            .endClass();
    luabridge::getGlobalNamespace(m_globalState)
            .beginClass<glm::vec4>("vec4")
            .addConstructor<void(*)(float)>()
            .addConstructor<void(*)(float, float, float, float)>()
            .endClass();

    luabridge::getGlobalNamespace(m_globalState)
            .beginNamespace("Debug")
            .addFunction("log", LuaScriptEngine::print)
            .endNamespace();

    luabridge::getGlobalNamespace(m_globalState)
            .beginNamespace("MaterialManager")
            .addFunction("loadImage", LuaScriptEngine::MtLloadImage)
            .addFunction("fromColor", LuaScriptEngine::MtLfromColor)
            .endNamespace();

    luabridge::getGlobalNamespace(m_globalState)
            .beginNamespace("ModelManager")
            .addFunction("loadModel", LuaScriptEngine::MdLloadModel)
            .addFunction("setMaterial", LuaScriptEngine::MdLsetMaterial)
            .endNamespace();

    luabridge::getGlobalNamespace(m_globalState)
            .beginNamespace("AudioManager")
            .addFunction("loadSound", LuaScriptEngine::AMloadSound)
            .addFunction("loadMusic", LuaScriptEngine::AMloadMusic)
            .addFunction("playSound", LuaScriptEngine::AMplaySound)
            .addFunction("playMusic", LuaScriptEngine::AMplayMusic)
            .endNamespace();

    {
        if(luaL_loadfile(m_globalState,
                         ServiceLocator::getResourceManager().getEnginePath("data/scripts/init.lua").c_str()) != LUA_OK)
        {
            ServiceLocator::getLogger().error(MODULE_NAME, "Lua load error: " + std::string());
            return;
        }
        if(lua_pcall(m_globalState, 0, 0, 0) != LUA_OK)
        {
            ServiceLocator::getLogger().error(MODULE_NAME, "Lua execution error: " + std::string());
            return;
        }
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
    Material *newMat = Material::create();
    newMat->setImage(ServiceLocator::getResourceManager().getEnginePath(path),
                     "img");
    newMat->init();
    ServiceLocator::getMatManager().addMaterial(name, newMat);
}

void LuaScriptEngine::MtLfromColor(const glm::vec4 &color, const std::string &name)
{
    Material *newMat = Material::create();
    newMat->setColor(color, "color");
    newMat->init();
    ServiceLocator::getMatManager().addMaterial(name, newMat);
}

void LuaScriptEngine::AMloadSound(const std::string &path, const std::string &name)
{
    ServiceLocator::getAudioManager().loadSound(path, name);
}

void LuaScriptEngine::AMplaySound(const std::string &name,
                                  float volume,
                                  float pitch,
                                  const glm::vec3 &pos)
{
    ServiceLocator::getAudioManager().playSound(name, SoundPropertiesInfo{volume, pitch, pos});
}

void LuaScriptEngine::AMloadMusic(const std::string &path, const std::string &name)
{
    ServiceLocator::getAudioManager().loadMusic(path, name);
}

void LuaScriptEngine::AMplayMusic(const std::string &name, float volume, float pitch)
{
    ServiceLocator::getAudioManager().playMusic(name, MusicPropertiesInfo{volume, pitch});
}

void LuaScriptEngine::print(const std::string &s)
{
    ServiceLocator::getLogger().info(MODULE_NAME, s);
}
