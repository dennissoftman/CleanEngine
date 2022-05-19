#ifndef LUASCRIPTENGINE_HPP
#define LUASCRIPTENGINE_HPP

#include "scriptengine.hpp"

#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>

#include "physicsmanager.hpp"

class LuaScriptEngine : public ScriptEngine
{
public:
    LuaScriptEngine();
    ~LuaScriptEngine();

    void init() override;

    // ModelLoader
    static void MdLloadModel(const std::string &path, const std::string &name);
    static void MdLsetMaterial(const std::string &model, const std::string &material);

    // MaterialLoader
    static void MtLloadImage(const std::string &path, const std::string &name);

    // Debug
    static void print(const std::string &s);

private:
    lua_State *m_globalState;
};

#endif // LUASCRIPTENGINE_HPP
