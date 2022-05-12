#include "luascriptengine.hpp"
#include "servicelocator.hpp"
#include <stdio_ext.h>

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
            .addFunction("print", LuaScriptEngine::print)
            .endNamespace();

    luabridge::getGlobalNamespace(m_globalState)
            .beginNamespace("Game")
            .addFunction("setWindowSize", LuaScriptEngine::setViewportSize)
            .endNamespace();

    luabridge::getGlobalNamespace(m_globalState)
            .beginNamespace("Graphics")
            .addFunction("createMaterial", LuaScriptEngine::GcreateMaterial)
            .endNamespace();
    //

    /*
    if(luaL_dofile(m_globalState, "data/scripts/init.lua") == 0)
    {
        lua_pcall(m_globalState, 0, 0, 0);
    }
    */
}

void LuaScriptEngine::setViewportSize(int width, int height)
{
    print("Resize: " + std::to_string(width) + "x" + std::to_string(height));
}

void LuaScriptEngine::GcreateMaterial(const std::string &name, const std::string &imgFile)
{
    ServiceLocator::getLogger().info(MODULE_NAME, "Importing " + name + " from '" + imgFile + "'");
}

void LuaScriptEngine::print(const std::string &s)
{
    ServiceLocator::getLogger().info(MODULE_NAME, s);
}
