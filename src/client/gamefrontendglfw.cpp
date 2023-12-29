#include <filesystem>
#include <toml++/toml.h>
#include <spdlog/spdlog.h>

#include "client/gamefrontendglfw.hpp"
#include "common/servicelocator.hpp"

#include "common/entities/staticmesh.hpp"
#include "common/luascriptengine.hpp"

#include "client/uimanager.hpp"

#include "server/gamebackend.hpp"


GameFrontend *GameFrontend::corePtr = nullptr;
static const char *MODULE_NAME = "GameFrontendGLFW";

GameFrontend *GameFrontend::create()
{
    return new GameFrontendGLFW();
}

GameFrontendGLFW::GameFrontendGLFW()
    : m_mainWindow(nullptr),
      m_elapsedTime(0), m_deltaTime(0)
{
    GameFrontend::corePtr = this;
}

GameFrontendGLFW::~GameFrontendGLFW()
{
    terminate();
}

void GameFrontendGLFW::onWindowResized(GLFWwindow *win, int width, int height)
{
    ServiceLocator::getRenderer().resize(glm::ivec2(width, height));
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
        spdlog::error(buff);
        throw std::runtime_error("Failed to init GLFW");
    }

    // load configs
    VideoMode videoMode;
    {
        std::string optionsPath = "./options.toml";
        if(std::filesystem::exists(optionsPath))
        {
            try
            {
                auto res = toml::parse_file(optionsPath);

                videoMode.setWidth(res["width"].value_or(1280))
                         .setHeight(res["height"].value_or(720))
                         .setFullscreen(res["fullscreen"].value_or(false))
                         .setVsync(res["vsync"].value_or(false))
                         .setSamples(res["msaaSamples"].value_or(1))
                         .setShadowMapResolution(res["shadowMapResolution"].value_or(512))
                         .setFSRScaling(res["fsrScaling"].value_or(1.0f))
                         .setRenderingBackend(res["renderingBackend"].value_or("vk"));
            }
            catch(const std::exception &e)
            {
                spdlog::warn("Config parsing error: " + std::string(e.what()));
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
            fout << "msaaSamples = 1" << std::endl;
            fout << "shadowMapResolution = 512" << std::endl;
            fout << "renderingBackend = \"vk\"" << std::endl;
            fout << "fsrScaling = 1.0" << std::endl;
        }
    }
    //

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    if(videoMode.fullscreen()) // windowed fullscreen
    {
        GLFWmonitor *mon = glfwGetPrimaryMonitor();
        const GLFWvidmode *vidMode = glfwGetVideoMode(mon);
        glfwWindowHint(GLFW_RED_BITS, vidMode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, vidMode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, vidMode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, vidMode->refreshRate);
        m_mainWindow = glfwCreateWindow(videoMode.width(), videoMode.height(),
                                        APP_NAME, mon, nullptr);
    }
    else
    {
        m_mainWindow = glfwCreateWindow(videoMode.width(), videoMode.height(),
                                        APP_NAME, nullptr, nullptr);
    }

    if(m_mainWindow == NULL)
    {
        const char *buff;
        glfwGetError(&buff);
        spdlog::error(buff);
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(m_mainWindow);

    glfwSetWindowSizeCallback(m_mainWindow, GameFrontendGLFW::onWindowResized);

    { // renderer
        m_mainRenderer = &ServiceLocator::getRenderer();
#ifdef _WIN32
        videoMode.setOSData(glfwGetWin32Window(m_mainWindow));
#elif __linux__
        videoMode.setOSData(glfwGetX11Window(m_mainWindow));
#endif
        m_mainRenderer->init(videoMode);
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
    SceneManager &sceneManager = ServiceLocator::getSceneManager();
    PhysicsManager& physManager = ServiceLocator::getPhysicsManager();

    while (!glfwWindowShouldClose(m_mainWindow))
    {
        m_deltaTime = glfwGetTime() - m_elapsedTime;
        m_elapsedTime = glfwGetTime();
        glfwPollEvents();

        physManager.update(m_deltaTime);
        // update scene
        sceneManager.update(m_deltaTime);

        // update audio
        audioManager.update(m_deltaTime);

        // update ui
        uiManager.update(m_deltaTime);

        // update renderer
        m_mainRenderer->update(m_deltaTime);

        // raise update event
        m_updateEvents(m_deltaTime);

        // update game services
        gameServices.update(m_deltaTime);

        // Draw objects on screen
        sceneManager.draw(m_mainRenderer);

        m_mainRenderer->draw();
    }
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
