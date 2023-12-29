#ifndef LUASCRIPTENGINE_HPP
#define LUASCRIPTENGINE_HPP

#include "common/scriptengine.hpp"

#define SOL_ALL_SAFETIES_ON 1

#include <limits>
#include <sol/sol.hpp>
#include <glm/vec3.hpp>

#include "client/inputmanager.hpp"

class UIElement;
struct Model3D;
struct FriendDetails;
class Scene3D;
class PhysicsBodyCreateInfo;

class LuaScriptEngine : public ScriptEngine
{
public:
    LuaScriptEngine();
    ~LuaScriptEngine();

    void init() override;

    // custom loaders
    static int luaRootLoader(lua_State *L);
private:
    // TODO: multiple states
    sol::state m_globalState;
};

#endif // LUASCRIPTENGINE_HPP
