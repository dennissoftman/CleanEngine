#ifndef SERVICELOCATOR_HPP
#define SERVICELOCATOR_HPP

#include "renderer.hpp"
#include "dummyrenderer.hpp"

#include "resourcemanager.hpp"

#include "materialmanager.hpp"

#include "modelmanager.hpp"

#include "logger.hpp"
#include "dummylogger.hpp"

#include "scenemanager.hpp"

#include "physicsmanager.hpp"
#include "dummyphysicsmanager.hpp"

#include "audiomanager.hpp"
#include "dummyaudiomanager.hpp"

#include "uimanager.hpp"
#include "dummyuimanager.hpp"

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

    // physics management
    static PhysicsManager &getPhysicsManager();
    static void setPhysicsManager(PhysicsManager *mgr);

    // audio management
    static AudioManager &getAudioManager();
    static void setAudioManager(AudioManager *mgr);

    // ui management
    static UIManager &getUIManager();
    static void setUIManager(UIManager *mgr);
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

    // physics management
    static PhysicsManager *m_physmgr;
    static DummyPhysicsManager m_defaultPhysMgr;

    // audio management
    static AudioManager *m_audiomgr;
    static DummyAudioManager m_defaultAudioMgr;

    // ui management
    static UIManager *m_uimgr;
    static DummyUIManager m_defaultUImgr;
};

#endif // SERVICELOCATOR_HPP
