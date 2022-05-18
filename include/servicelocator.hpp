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

class ServiceLocator
{
public:
    static void init();

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

    // logging
    static Logger &getLogger();
    static void setLogger(Logger *logger);

    // unregister all services
    static void clear();
private:
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

    // logging
    static Logger *m_logger;
    static DummyLogger m_defaultLogger;

    //
};

#endif // SERVICELOCATOR_HPP
