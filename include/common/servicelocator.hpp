#ifndef SERVICELOCATOR_HPP
#define SERVICELOCATOR_HPP

#include "client/renderer.hpp"
#include "client/dummyrenderer.hpp"

#include "common/resourcemanager.hpp"

#include "client/materialmanager.hpp"

#include "common/modelmanager.hpp"

#include "common/logger.hpp"
#include "common/dummylogger.hpp"

#include "server/scenemanager.hpp"

#include "common/eventmanager.hpp"

#include "server/physicsmanager.hpp"
#include "server/dummyphysicsmanager.hpp"

#include "client/audiomanager.hpp"
#include "client/dummyaudiomanager.hpp"

#include "client/uimanager.hpp"
#include "client/dummyuimanager.hpp"

#include "common/scriptengine.hpp"
#include "common/dummyscriptengine.hpp"

#include "common/gameservices.hpp"
#include "common/dummyservices.hpp"

class ServiceLocator
{
public:
    // register all services
    static void init();
    // unregister all services
    static void terminate();

    // logging
    static Logger &getLogger();
    static void setLogger(Logger *logger);

    // rendering
    static Renderer &getRenderer();
    static void setRenderer(Renderer *rend);

    // resource manager
    static ResourceManager &getResourceManager();
    static void setResourceManager(ResourceManager *mgr);

    // materials
    static MaterialManager &getMatManager();
    static void setMatManager(MaterialManager *mgr);

    // models
    static ModelManager &getModelManager();
    static void setModelManager(ModelManager *mgr);

    // scene management
    static SceneManager &getSceneManager();
    static void setSceneManager(SceneManager *mgr); // pretty useless

    static EventManager &getEventManager();
    static void setEventManager(EventManager *mgr);

    // physics management
    static PhysicsManager &getPhysicsManager();
    static void setPhysicsManager(PhysicsManager *mgr);

    // audio management
    static AudioManager &getAudioManager();
    static void setAudioManager(AudioManager *mgr);

    // ui management
    static UIManager &getUIManager();
    static void setUIManager(UIManager *mgr);

    // scripts
    static ScriptEngine &getScriptEngine();
    static void setScriptEngine(ScriptEngine *eng);

    // game services
    static GameServices &getGameServices();
    static void setGameServices(GameServices *svs);
private:
    // logging
    static Logger *m_logger;
    static DummyLogger m_defaultLogger;

    // rendering
    static Renderer *m_renderer;
    static DummyRenderer m_defaultRenderer;

    // resources
    static ResourceManager *m_resmgr;

    // materials
    static MaterialManager *m_matmgr;

    // models
    static ModelManager *m_mdlmgr;

    // scene management
    static SceneManager *m_scnmgr;

    // event management
    static EventManager *m_evtmgr;

    // physics management
    static PhysicsManager *m_physmgr;
    static DummyPhysicsManager m_defaultPhysMgr;

    // audio management
    static AudioManager *m_audiomgr;
    static DummyAudioManager m_defaultAudioMgr;

    // ui management
    static UIManager *m_uimgr;
    static DummyUIManager m_defaultUImgr;

    // scripts
    static ScriptEngine *m_scengine;
    static DummyScriptEngine m_defaultScriptEngine;

    // game services
    static GameServices *m_gamesvcs;
    static DummyServices m_defaultGameSvcs;
};

#endif // SERVICELOCATOR_HPP
