#include "client/gameclientglfw.hpp"
#include "common/servicelocator.hpp"

#include "common/entities/staticmesh.hpp"
#include "common/luascriptengine.hpp"

#include "client/uimanager.hpp"
#ifdef UI_IMGUI
#include "imgui_impl_glfw.h"
#endif

GameClientGLFW *GameClientGLFW::corePtr = nullptr;
static const char *MODULE_NAME = "EngineCoreGLFW";

GameClientGLFW::GameClientGLFW()
    : m_mainWindow(nullptr),
      m_elapsedTime(0), m_deltaTime(0),
      m_scriptEngine(nullptr),
      m_windowSize(glm::ivec2(1920, 1080))
{
    assert(corePtr == nullptr && "Instance is already running");
    corePtr = this;
}

GameClientGLFW::~GameClientGLFW()
{

}

void GameClientGLFW::onWindowResized(GLFWwindow *win, int width, int height)
{
    (void)win;
    GameClientGLFW::corePtr->m_mainRenderer->resize(glm::ivec2(width, height));
}

void GameClientGLFW::init()
{
    if(glfwInit() != GLFW_TRUE)
    {
        const char *buff;
        glfwGetError(&buff);
        ServiceLocator::getLogger().info(MODULE_NAME, buff);
        throw std::runtime_error("Failed to init GLFW");
    }

    std::string appName;
#ifdef RENDERER_OPENGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    glfwWindowHint(GLFW_SAMPLES, 2);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    appName = "CleanEngineGL";

#elif RENDERER_VULKAN
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    appName = "CleanEngineVk";
#else
#error No graphics API specified
#endif

    m_mainWindow = glfwCreateWindow(m_windowSize.x, m_windowSize.y,
                                    appName.c_str(), nullptr, nullptr);
    if(m_mainWindow == NULL)
    {
        const char *buff;
        glfwGetError(&buff);
        ServiceLocator::getLogger().info(MODULE_NAME, buff);
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(m_mainWindow);

    glfwSetWindowSizeCallback(m_mainWindow, GameClientGLFW::onWindowResized);

    glfwSwapInterval(0); // disable vsync

    { // renderer
        m_mainRenderer = Renderer::create();
        m_mainRenderer->init(VideoMode(m_windowSize.x, m_windowSize.y));
        ServiceLocator::setRenderer(m_mainRenderer);
    }

    { // events

    }

    { // ui
        UIManager *uimgr = UIManager::create();
        uimgr->init();
        ServiceLocator::setUIManager(uimgr);
    }

    { // scene
        Scene3D &activeScene = ServiceLocator::getSceneManager().activeScene();
        activeScene.getCamera().setPosition(glm::vec3(0, 2.f, 0));
        activeScene.getCamera().setRotation(glm::vec3(glm::pi<float>()/2.f, 0, 0));
    }

    { // scripts
        m_scriptEngine = new LuaScriptEngine();
        m_scriptEngine->init();
    }
}

void GameClientGLFW::terminate()
{
    // TODO: move to ServiceLocator
    delete m_scriptEngine;
    m_scriptEngine = nullptr;
    //
    ServiceLocator::terminate();
    glfwSetInputMode(m_mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwTerminate();
}

void GameClientGLFW::mainLoop()
{
    SceneManager &sceneManager = ServiceLocator::getSceneManager();
    PhysicsManager &physicsManager = ServiceLocator::getPhysicsManager();
    AudioManager &audioManager = ServiceLocator::getAudioManager();
    UIManager &uiManager = ServiceLocator::getUIManager();

    while (!glfwWindowShouldClose(m_mainWindow))
    {
        m_deltaTime = glfwGetTime() - m_elapsedTime;
        m_elapsedTime = glfwGetTime();
        glfwPollEvents();

        // update physics
        physicsManager.update(m_deltaTime);

        {
            Camera3D &cam = sceneManager.activeScene().getCamera();
            float cam_speed = 5.f;
            if(glfwGetKey(m_mainWindow, GLFW_KEY_W) == GLFW_PRESS)
                cam.move(cam.frontVector() * cam_speed * (float)m_deltaTime);
            else if(glfwGetKey(m_mainWindow, GLFW_KEY_S) == GLFW_PRESS)
                cam.move(-cam.frontVector() * cam_speed * (float)m_deltaTime);

            if(glfwGetKey(m_mainWindow, GLFW_KEY_D) == GLFW_PRESS)
                cam.move(cam.rightVector() * cam_speed * (float)m_deltaTime);
            else if(glfwGetKey(m_mainWindow, GLFW_KEY_A) == GLFW_PRESS)
                cam.move(-cam.rightVector() * cam_speed * (float)m_deltaTime);
        }

        // update scene
        sceneManager.activeScene().update(m_deltaTime);

        // update gui
        uiManager.update(m_deltaTime);

        // update sounds
        audioManager.update(m_deltaTime);

        // Draw objects on screen
        sceneManager.activeScene().draw(m_mainRenderer);

        m_mainRenderer->draw();

        // =====================================================================================
#ifdef RENDERER_OPENGL
        glfwSwapBuffers(m_mainWindow);
#endif
    }
}

double GameClientGLFW::getDeltaTime() const
{
    return m_deltaTime;
}

double GameClientGLFW::getElapsedTime() const
{
    return m_elapsedTime;
}

GLFWwindow *GameClientGLFW::getWindowPtr() const
{
    return m_mainWindow;
}
