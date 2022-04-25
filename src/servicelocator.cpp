#include "servicelocator.hpp"
#include <memory>

Renderer *ServiceLocator::m_renderer = nullptr;
DummyRenderer ServiceLocator::m_defaultRenderer = DummyRenderer();

MaterialManager *ServiceLocator::m_matmgr = nullptr;
DummyMaterialManager ServiceLocator::m_defaultMatManager = DummyMaterialManager();

DummyLogger ServiceLocator::m_defaultLogger = DummyLogger();
Logger *ServiceLocator::m_logger = nullptr;

void ServiceLocator::init()
{
    m_renderer = &m_defaultRenderer;
    m_matmgr = &m_defaultMatManager;
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
    if(m_renderer != &m_defaultRenderer)
        delete m_renderer;

    if(m_matmgr != &m_defaultMatManager)
        delete m_matmgr;

    if(m_logger != &m_defaultLogger)
        delete m_logger;
}
