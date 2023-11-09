#include "client/gamefrontendglfw.hpp"
#include "common/servicelocator.hpp"

#include "common/entities/staticmesh.hpp"
#include "common/luascriptengine.hpp"

#include "client/uimanager.hpp"
#ifdef UI_IMGUI
#include "imgui_impl_glfw.h"
#endif

#include "server/gamebackend.hpp"

#include "common/cfgpath.hpp"
#include <filesystem>
#include <toml++/toml.h>

GameFrontend *GameFrontend::corePtr = nullptr;
static const char *MODULE_NAME = "GameFrontendGLFW";

GameFrontend *GameFrontend::create()
{
    return new GameFrontendGLFW();
}

GameFrontendGLFW::GameFrontendGLFW()
    : m_mainWindow(nullptr),
      m_elapsedTime(0), m_deltaTime(0),
      m_windowSize(glm::ivec2(1280, 720))
{
    GameFrontend::corePtr = this;
}

GameFrontendGLFW::~GameFrontendGLFW()
{
    terminate();
}

void GameFrontendGLFW::onWindowResized(GLFWwindow *win, int width, int height)
{
    // TODO: resize window
}

void GameFrontendGLFW::onKeyboardEvent(GLFWwindow *win, int key, int scancode, int action, int mods)
{
    ServiceLocator::getEventManager().keyboardCallback(key,
                                                       scancode,
                                                       (action == GLFW_PRESS) ? 1 : ((action == GLFW_RELEASE) ? -1 : 0),
                                                       0);
}

void GameFrontendGLFW::onMouseButtonEvent(GLFWwindow *win, int button, int action, int mods)
{
    ServiceLocator::getEventManager().mouseButtonCallback(button,
                                                          (action == GLFW_PRESS) ? 1 : ((action == GLFW_RELEASE) ? -1 : 0));
}

void GameFrontendGLFW::onMousePositionEvent(GLFWwindow *win, double mx, double my)
{
    ServiceLocator::getEventManager().mousePositionCallback(static_cast<int>(mx),
                                                            static_cast<int>(my));
}

void GameFrontendGLFW::onMouseScrollEvent(GLFWwindow *win, double sx, double sy)
{
    ServiceLocator::getEventManager().mouseScrollCallback(static_cast<int>(sx),
                                                          static_cast<int>(sy));
}

void GameFrontendGLFW::onJoystickEvent(int jid, int event)
{
    ServiceLocator::getEventManager().joystickEventCallback(jid, event);
}

void GameFrontendGLFW::init()
{
    if(glfwInit() != GLFW_TRUE)
    {
        const char *buff;
        glfwGetError(&buff);
        ServiceLocator::getLogger().info(MODULE_NAME, buff);
        throw std::runtime_error("Failed to init GLFW");
    }

    // load configs
    {
        std::string cPath = CfgPath::configDirectoryPath(APP_NAME);
        std::string optionsPath = cPath + "options.toml";
        if(std::filesystem::exists(optionsPath))
        {
            try
            {
                auto res = toml::parse_file(optionsPath);

                int scrWidth  = res["width"].value_or(1280);
                int scrHeight = res["height"].value_or(720);
                bool fullscreen = res["fullscreen"].value_or(false);
                bool vsync    = res["vsync"].value_or(false);
                int fpsCap    = res["fpsCap"].value_or(300);
                int samples   = res["msaaSamples"].value_or(1);

                m_windowSize    = glm::ivec2(scrWidth, scrHeight);
                m_windowFullscreen = fullscreen;
                m_windowVSync   = vsync;
                m_windowFpsCap  = fpsCap;
                m_windowSamples = samples;
            }
            catch(const std::exception &e)
            {
                ServiceLocator::getLogger().error(MODULE_NAME, "Config parsing error: " + std::string(e.what()));
            }
        }
        else
        {
            std::ofstream fout;
            fout.open(optionsPath);
            fout << "width = 1280" << std::endl;
            fout << "height = 720" << std::endl;
            fout << "fullscreen = false" << std::endl;
            fout << "vsync = false" << std::endl;
            fout << "fpsCap = 300" << std::endl;
            fout << "msaaSamples = 1" << std::endl;
        }
    }
    //

#ifdef RENDERER_OPENGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 32);
    glfwWindowHint(GLFW_SAMPLES, m_windowSamples);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

#elif RENDERER_VULKAN
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#else
#error No graphics API specified
#endif

    if(m_windowFullscreen) // windowed fullscreen
    {
        GLFWmonitor *mon = glfwGetPrimaryMonitor();
        const GLFWvidmode *vidMode = glfwGetVideoMode(mon);
        glfwWindowHint(GLFW_RED_BITS, vidMode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, vidMode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, vidMode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, vidMode->refreshRate);
        m_mainWindow = glfwCreateWindow(m_windowSize.x, m_windowSize.y,
                                        APP_NAME, mon, nullptr);
    }
    else
    {
        m_mainWindow = glfwCreateWindow(m_windowSize.x, m_windowSize.y,
                                        APP_NAME, nullptr, nullptr);
    }

    if(m_mainWindow == NULL)
    {
        const char *buff;
        glfwGetError(&buff);
        ServiceLocator::getLogger().info(MODULE_NAME, buff);
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(m_mainWindow);

    glfwSetWindowSizeCallback(m_mainWindow, GameFrontendGLFW::onWindowResized);

    if(m_windowVSync)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);

    { // renderer
        m_mainRenderer = &ServiceLocator::getRenderer();
        m_mainRenderer->init(VideoMode(m_windowSize.x, m_windowSize.y,
                                       m_windowFullscreen, m_windowVSync,
                                       m_windowSamples));
    }

    { // events
        glfwSetKeyCallback(m_mainWindow, GameFrontendGLFW::onKeyboardEvent);
        glfwSetMouseButtonCallback(m_mainWindow, GameFrontendGLFW::onMouseButtonEvent);
        glfwSetCursorPosCallback(m_mainWindow, GameFrontendGLFW::onMousePositionEvent);
        glfwSetScrollCallback(m_mainWindow, GameFrontendGLFW::onMouseScrollEvent);
        glfwSetJoystickCallback(GameFrontendGLFW::onJoystickEvent);
    }

    // ui
    ServiceLocator::getUIManager().init(m_mainRenderer);

    // scripts
    ServiceLocator::getScriptEngine().init();
}

void GameFrontendGLFW::run()
{
    mainLoop();
}

void GameFrontendGLFW::terminate()
{
    glfwTerminate();
}

void GameFrontendGLFW::mainLoop()
{
    UIManager &uiManager = ServiceLocator::getUIManager();
    AudioManager &audioManager = ServiceLocator::getAudioManager();
    GameServices &gameServices = ServiceLocator::getGameServices();

    GameBackend *backend = GameBackend::corePtr;
    while (!glfwWindowShouldClose(m_mainWindow))
    {
        m_deltaTime = glfwGetTime() - m_elapsedTime;
        m_elapsedTime = glfwGetTime();
        glfwPollEvents();

        backend->update(m_deltaTime);

        // update scene
        m_currentScene.update(m_deltaTime);

        // update audio
        audioManager.update(m_deltaTime);

        // update ui
        uiManager.update(m_deltaTime);

        // raise update event
        m_updateEvents(m_deltaTime);

        // update game services
        gameServices.update(m_deltaTime);

        // Draw objects on screen
        m_currentScene.draw(m_mainRenderer);

        m_mainRenderer->draw();

        // =====================================================================================
#ifdef RENDERER_OPENGL
        glfwSwapBuffers(m_mainWindow);
#endif
    }
}

Scene3D &GameFrontendGLFW::getScene()
{
    return m_currentScene;
}

double GameFrontendGLFW::getDeltaTime() const
{
    return m_deltaTime;
}

double GameFrontendGLFW::getElapsedTime() const
{
    return m_elapsedTime;
}

void GameFrontendGLFW::lockCursor()
{
    glfwSetInputMode(m_mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void GameFrontendGLFW::unlockCursor()
{
    glfwSetInputMode(m_mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

GLFWwindow *GameFrontendGLFW::getWindowPtr() const
{
    return m_mainWindow;
}

void GameFrontendGLFW::updateSubscribe(const std::function<void (double)> &callb)
{
    m_updateEvents.connect(callb);
}
