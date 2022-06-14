#include "client/gameclientglfw.hpp"
#include "common/servicelocator.hpp"

#include "common/entities/staticmesh.hpp"
#include "common/luascriptengine.hpp"

#include "client/uimanager.hpp"
#ifdef UI_IMGUI
#include "imgui_impl_glfw.h"
#endif

GameClient *GameClient::corePtr = nullptr;
static const char *MODULE_NAME = "EngineCoreGLFW";

GameClient *GameClient::create()
{
    return new GameClientGLFW();
}

GameClientGLFW::GameClientGLFW()
    : m_mainWindow(nullptr),
      m_elapsedTime(0), m_deltaTime(0),
      m_windowSize(glm::ivec2(1920, 1080))
{
    GameClient::corePtr = this;
}

GameClientGLFW::~GameClientGLFW()
{
    terminate();
}

void GameClientGLFW::onWindowResized(GLFWwindow *win, int width, int height)
{
    (void)win;
    (void)width;
    (void)height;
}

void GameClientGLFW::onKeyboardEvent(GLFWwindow *win, int key, int scancode, int action, int mods)
{
    (void)win;
    (void)mods;

    ServiceLocator::getEventManager().keyboardCallback(key,
                                                       scancode,
                                                       (action == GLFW_PRESS) ? 1 : ((action == GLFW_RELEASE) ? -1 : 0),
                                                       0);
}

void GameClientGLFW::onMouseButtonEvent(GLFWwindow *win, int button, int action, int mods)
{
    (void)win;
    (void)mods;

    ServiceLocator::getEventManager().mouseButtonCallback(button,
                                                          (action == GLFW_PRESS) ? 1 : ((action == GLFW_RELEASE) ? -1 : 0));
}

void GameClientGLFW::onMousePositionEvent(GLFWwindow *win, double mx, double my)
{
    (void)win;
    ServiceLocator::getEventManager().mousePositionCallback(static_cast<int>(mx),
                                                            static_cast<int>(my));
}

void GameClientGLFW::onMouseScrollEvent(GLFWwindow *win, double sx, double sy)
{
    (void)win;
    ServiceLocator::getEventManager().mouseScrollCallback(static_cast<int>(sx),
                                                          static_cast<int>(sy));
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
        m_mainRenderer = &ServiceLocator::getRenderer();
        m_mainRenderer->init(VideoMode(m_windowSize.x, m_windowSize.y));
    }

    { // events
        glfwSetKeyCallback(m_mainWindow, GameClientGLFW::onKeyboardEvent);
        glfwSetMouseButtonCallback(m_mainWindow, GameClientGLFW::onMouseButtonEvent);
        glfwSetCursorPosCallback(m_mainWindow, GameClientGLFW::onMousePositionEvent);
        glfwSetScrollCallback(m_mainWindow, GameClientGLFW::onMouseScrollEvent);
    }

    // ui
    ServiceLocator::getUIManager().init(m_mainRenderer);

    { // scene
        Scene3D &activeScene = ServiceLocator::getSceneManager().activeScene();
        activeScene.getCamera().setPosition(glm::vec3(0, 2.f, 0));
    }

    // scripts
    ServiceLocator::getScriptEngine().init();
}

void GameClientGLFW::run()
{
    mainLoop();
}

void GameClientGLFW::terminate()
{
    ServiceLocator::terminate();
    glfwTerminate();
}

void GameClientGLFW::mainLoop()
{
    SceneManager &sceneManager = ServiceLocator::getSceneManager();
    EventManager &eventManager = ServiceLocator::getEventManager();
    AudioManager &audioManager = ServiceLocator::getAudioManager();
    UIManager &uiManager = ServiceLocator::getUIManager();

    while (!glfwWindowShouldClose(m_mainWindow))
    {
        m_deltaTime = glfwGetTime() - m_elapsedTime;
        m_elapsedTime = glfwGetTime();
        glfwPollEvents();

        // update events
        eventManager.update(m_deltaTime);

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

void GameClientGLFW::lockCursor()
{
    glfwSetInputMode(m_mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void GameClientGLFW::unlockCursor()
{
    glfwSetInputMode(m_mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

GLFWwindow *GameClientGLFW::getWindowPtr() const
{
    return m_mainWindow;
}
