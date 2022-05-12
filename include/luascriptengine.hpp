#ifndef LUASCRIPTENGINE_HPP
#define LUASCRIPTENGINE_HPP

#include "scriptengine.hpp"

#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>

class LuaScriptEngine : public ScriptEngine
{
public:
    LuaScriptEngine();
    ~LuaScriptEngine();

    void init() override;

    // Game
    static void setViewportSize(int width, int height);

    // Graphics
    static void GcreateMaterial(const std::string &name, const std::string &imgFile);

    // Debug
    static void print(const std::string &s);

private:
    lua_State *m_globalState;
};

#endif // LUASCRIPTENGINE_HPP
