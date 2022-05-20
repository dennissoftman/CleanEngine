#ifndef LUASCRIPTENGINE_HPP
#define LUASCRIPTENGINE_HPP

#include "scriptengine.hpp"

#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>

#include "audiomanager.hpp"

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
    static void MtLfromColor(const glm::vec4 &color, const std::string &name);

    // AudioManager
    static void AMloadSound(const std::string &path, const std::string &name);
    static void AMplaySound(const std::string &name,
                            float volume=1.f,
                            float pitch=1.f,
                            const glm::vec3 &pos=glm::vec3(0,0,0));

    static void AMloadMusic(const std::string &path, const std::string &name);
    static void AMplayMusic(const std::string &name,
                            float volume=1.f,
                            float pitch=1.f);

    // Debug
    static void print(const std::string &s);

private:
    lua_State *m_globalState;
};

#endif // LUASCRIPTENGINE_HPP
