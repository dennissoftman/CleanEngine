#ifndef LUASCRIPTENGINE_HPP
#define LUASCRIPTENGINE_HPP

#include "scriptengine.hpp"

#define SOL_ALL_SAFETIES_ON 1

#include <limits>
#include <sol/sol.hpp>

class Model3D;
class Scene3D;

class LuaScriptEngine : public ScriptEngine
{
public:
    LuaScriptEngine();
    ~LuaScriptEngine();

    void init() override;

    // Logging
    static void Debug_log(const std::string &msg);
    static void Debug_warning(const std::string &msg);
    static void Debug_error(const std::string &msg);

    // Material manager
    static void MaterialManager_loadImage(const std::string &path, const std::string &name);

    // Model manager
    static void ModelManager_loadModel(const std::string &path, const std::string &name);
    static const Model3D *ModelManager_getModel(const std::string &name);

    // Audio manager
    static void AudioManager_loadSound(const std::string &path, const std::string &name);

    // Scene manager
    static Scene3D &SceneManager_getActiveScene();
private:
    sol::state m_globalState;
};

#endif // LUASCRIPTENGINE_HPP
