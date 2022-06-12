#include "common/servicelocator.hpp"

#include <memory>

//
DummyLogger ServiceLocator::m_defaultLogger = DummyLogger();
Logger *ServiceLocator::m_logger = nullptr;

ResourceManager *ServiceLocator::m_resmgr = nullptr;

Renderer *ServiceLocator::m_renderer = nullptr;
DummyRenderer ServiceLocator::m_defaultRenderer = DummyRenderer();

MaterialManager *ServiceLocator::m_matmgr = nullptr;

ModelManager *ServiceLocator::m_mdlmgr = nullptr;

SceneManager *ServiceLocator::m_scnmgr = nullptr;

EventManager *ServiceLocator::m_evtmgr = nullptr;

PhysicsManager *ServiceLocator::m_physmgr = nullptr;
DummyPhysicsManager ServiceLocator::m_defaultPhysMgr = DummyPhysicsManager();

AudioManager *ServiceLocator::m_audiomgr = nullptr;
DummyAudioManager ServiceLocator::m_defaultAudioMgr = DummyAudioManager();

UIManager *ServiceLocator::m_uimgr = nullptr;
DummyUIManager ServiceLocator::m_defaultUImgr = DummyUIManager();

ScriptEngine *ServiceLocator::m_scengine = nullptr;
DummyScriptEngine ServiceLocator::m_defaultScriptEngine = DummyScriptEngine();

GameServices *ServiceLocator::m_gamesvcs = nullptr;
DummyServices ServiceLocator::m_defaultGameSvcs = DummyServices();

//

void ServiceLocator::init()
{
    m_logger = &m_defaultLogger;

    m_resmgr = new ResourceManager();
    m_renderer = &m_defaultRenderer;
    m_matmgr = new MaterialManager();
    m_mdlmgr = new ModelManager();
    m_scnmgr = new SceneManager();
    m_evtmgr = new EventManager();
    m_physmgr = &m_defaultPhysMgr;
    m_audiomgr = &m_defaultAudioMgr;
    m_uimgr = &m_defaultUImgr;
    m_scengine = &m_defaultScriptEngine;
}

void ServiceLocator::terminate()
{
    if(m_gamesvcs != &m_defaultGameSvcs)
        delete m_gamesvcs;
    m_gamesvcs = &m_defaultGameSvcs;

    if(m_scengine != &m_defaultScriptEngine)
        delete m_scengine;
    m_scengine = &m_defaultScriptEngine;

    if(m_uimgr != &m_defaultUImgr)
        delete m_uimgr;
    m_uimgr = &m_defaultUImgr;

    if(m_audiomgr != &m_defaultAudioMgr)
        delete m_audiomgr;
    m_audiomgr = &m_defaultAudioMgr;

    if(m_physmgr != &m_defaultPhysMgr)
        delete m_physmgr;
    m_physmgr = &m_defaultPhysMgr;

    delete m_evtmgr;
    m_evtmgr = nullptr;

    delete m_scnmgr;
    m_scnmgr = nullptr;

    delete m_mdlmgr;
    m_mdlmgr = nullptr;

    delete m_matmgr;
    m_matmgr = nullptr;

    if(m_renderer != &m_defaultRenderer)
        delete m_renderer;
    m_renderer = &m_defaultRenderer;

    delete m_resmgr;
    m_resmgr = nullptr;

    if(m_logger != &m_defaultLogger)
        delete m_logger;
    m_logger = &m_defaultLogger;
}

Renderer &ServiceLocator::getRenderer()
{
    return *m_renderer;
}

void ServiceLocator::setRenderer(Renderer *rend)
{
    if(rend)
        m_renderer = rend;
}

ResourceManager &ServiceLocator::getResourceManager()
{
    return *m_resmgr;
}

void ServiceLocator::setResourceManager(ResourceManager *mgr)
{
    if(mgr)
        m_resmgr = mgr;
}

MaterialManager &ServiceLocator::getMatManager()
{
    return *m_matmgr;
}

void ServiceLocator::setMatManager(MaterialManager *mgr)
{
    if(mgr)
        m_matmgr = mgr;
}

ModelManager &ServiceLocator::getModelManager()
{
    return *m_mdlmgr;
}

void ServiceLocator::setModelManager(ModelManager *mgr)
{
    if(mgr)
        m_mdlmgr = mgr;
}

SceneManager &ServiceLocator::getSceneManager()
{
    return *m_scnmgr;
}

void ServiceLocator::setSceneManager(SceneManager *mgr)
{
    if(mgr)
        m_scnmgr = mgr;
}

EventManager &ServiceLocator::getEventManager()
{
    return *m_evtmgr;
}

void ServiceLocator::setEventManager(EventManager *mgr)
{
    if(mgr)
        m_evtmgr = mgr;
}

PhysicsManager &ServiceLocator::getPhysicsManager()
{
    return *m_physmgr;
}

void ServiceLocator::setPhysicsManager(PhysicsManager *mgr)
{
    if(mgr)
        m_physmgr = mgr;
}

AudioManager &ServiceLocator::getAudioManager()
{
    return *m_audiomgr;
}

void ServiceLocator::setAudioManager(AudioManager *mgr)
{
    if(mgr)
        m_audiomgr = mgr;
}

UIManager &ServiceLocator::getUIManager()
{
    return *m_uimgr;
}

void ServiceLocator::setUIManager(UIManager *mgr)
{
    if(mgr)
        m_uimgr = mgr;
}

ScriptEngine &ServiceLocator::getScriptEngine()
{
    return *m_scengine;
}

void ServiceLocator::setScriptEngine(ScriptEngine *eng)
{
    if(eng)
        m_scengine = eng;
}

GameServices &ServiceLocator::getGameServices()
{
    return *m_gamesvcs;
}

void ServiceLocator::setGameServices(GameServices *svs)
{
    if(svs)
        m_gamesvcs = svs;
}

Logger &ServiceLocator::getLogger()
{
    return *m_logger;
}

void ServiceLocator::setLogger(Logger *logger)
{
    if(logger)
        m_logger = logger;
}
