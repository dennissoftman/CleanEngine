#include "common/servicelocator.hpp"

#include <memory>

//
DummyLogger ServiceLocator::m_defaultLogger = DummyLogger();
Logger *ServiceLocator::m_logger = nullptr;

ResourceManager *ServiceLocator::m_resmgr = nullptr;

Renderer *ServiceLocator::m_renderer = nullptr;

MaterialManager *ServiceLocator::m_matmgr = nullptr;

ModelManager *ServiceLocator::m_mdlmgr = nullptr;

SceneManager *ServiceLocator::m_scnmgr = nullptr;

InputManager *ServiceLocator::m_evtmgr = nullptr;

PhysicsManager *ServiceLocator::m_physmgr = nullptr;

AudioManager *ServiceLocator::m_audiomgr = nullptr;

UIManager *ServiceLocator::m_uimgr = nullptr;

ScriptEngine *ServiceLocator::m_scengine = nullptr;

GameServices *ServiceLocator::m_gamesvcs = nullptr;

//

void ServiceLocator::init()
{
    m_logger = &m_defaultLogger;

    m_resmgr = new ResourceManager();
    m_renderer = Renderer::create();
    m_matmgr = new MaterialManager();
    m_mdlmgr = new ModelManager();
    m_scnmgr = new SceneManager();
    m_evtmgr = new InputManager();
    m_physmgr = PhysicsManager::create();
    m_audiomgr = AudioManager::create();
    m_uimgr = UIManager::create();
    m_scengine = ScriptEngine::create();
    m_gamesvcs = GameServices::create();
}

void ServiceLocator::terminate()
{
    delete m_gamesvcs;
    m_gamesvcs = nullptr;

    delete m_scengine;
    m_scengine = nullptr;

    delete m_uimgr;
    m_uimgr = nullptr;

    delete m_audiomgr;
    m_audiomgr = nullptr;

    delete m_physmgr;
    m_physmgr = nullptr;

    delete m_evtmgr;
    m_evtmgr = nullptr;

    delete m_scnmgr;
    m_scnmgr = nullptr;

    delete m_mdlmgr;
    m_mdlmgr = nullptr;

    delete m_matmgr;
    m_matmgr = nullptr;

    delete m_renderer;
    m_renderer = nullptr;

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

InputManager &ServiceLocator::getEventManager()
{
    return *m_evtmgr;
}

void ServiceLocator::setEventManager(InputManager *mgr)
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
