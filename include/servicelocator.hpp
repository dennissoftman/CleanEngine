#ifndef SERVICELOCATOR_HPP
#define SERVICELOCATOR_HPP

#include "renderer.hpp"
#include "dummyrenderer.hpp"

#include "materialmanager.hpp"
#include "dummymaterialmanager.hpp"

#include "logger.hpp"
#include "dummylogger.hpp"

class ServiceLocator
{
public:
    static void init();

    // rendering
    static Renderer &getRenderer();
    static void setRenderer(Renderer *rend);

    // materials
    static MaterialManager &getMatManager();
    static void setMatManager(MaterialManager *mgr);

    // logging
    static Logger &getLogger();
    static void setLogger(Logger *logger);

    // unregister all services
    static void clear();
private:
    // rendering
    static Renderer *m_renderer;
    static DummyRenderer m_defaultRenderer;

    // materials
    static MaterialManager *m_matmgr;
    static DummyMaterialManager m_defaultMatManager;

    // logging
    static Logger *m_logger;
    static DummyLogger m_defaultLogger;

    //
};

#endif // SERVICELOCATOR_HPP
