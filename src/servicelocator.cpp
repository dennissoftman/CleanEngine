#include "servicelocator.hpp"
#include <memory>

Renderer *ServiceLocator::m_renderer = nullptr;
DummyRenderer ServiceLocator::m_defaultRenderer = DummyRenderer();

MaterialManager *ServiceLocator::m_matmgr = nullptr;

ModelManager *ServiceLocator::m_mdlmgr = nullptr;

SceneManager *ServiceLocator::m_scnmgr = nullptr;

DummyLogger ServiceLocator::m_defaultLogger = DummyLogger();
Logger *ServiceLocator::m_logger = nullptr;

void ServiceLocator::init()
{
    m_renderer = &m_defaultRenderer;
    m_matmgr = new MaterialManager();
    m_mdlmgr = new ModelManager();
    m_scnmgr = new SceneManager();
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

Logger &ServiceLocator::getLogger()
{
    return *m_logger;
}

void ServiceLocator::setLogger(Logger *logger)
{
    if(logger)
        m_logger = logger;
}

void ServiceLocator::clear()
{
    delete m_scnmgr;
    delete m_mdlmgr;
    delete m_matmgr;

    if(m_renderer != &m_defaultRenderer)
        delete m_renderer;

    if(m_logger != &m_defaultLogger)
        delete m_logger;
}
