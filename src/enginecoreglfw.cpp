#include "enginecoreglfw.hpp"
#include "servicelocator.hpp"

#include <IL/il.h>
#include <IL/ilu.h>

#include "staticmesh.hpp"
#include "luascriptengine.hpp"

#ifdef UI_IMGUI
#include "imgui_impl_glfw.h"
#endif

EngineCoreGLFW *EngineCoreGLFW::corePtr = nullptr;
static const char *MODULE_NAME = "EngineCoreGLFW";

EngineCoreGLFW::EngineCoreGLFW()
    : m_mainWindow(nullptr),
      m_elapsedTime(0), m_deltaTime(0),
      m_scriptEngine(nullptr),
      m_windowSize(glm::ivec2(1920, 1080))
{
    assert(corePtr == nullptr && "Instance is already running");
    corePtr = this;
}

EngineCoreGLFW::~EngineCoreGLFW()
{

}

void EngineCoreGLFW::onWindowResized(GLFWwindow *win, int width, int height)
{
    (void)win;
    EngineCoreGLFW::corePtr->m_mainRenderer->resize(glm::ivec2(width, height));
}

static glm::ivec2 OldCursorPos = glm::ivec2(0,0);
void testMouseBind(GLFWwindow *win, double xpos, double ypos)
{
    (void)win;

    {
        Camera3D &cam = ServiceLocator::getSceneManager().activeScene().getCamera();
        float sens = 0.005f;

        cam.rotate(sens * (OldCursorPos.y - ypos), glm::vec3(1, 0, 0));
        cam.rotate(sens * (xpos - OldCursorPos.x), glm::vec3(0, 1, 0));
    }

    OldCursorPos = glm::ivec2(xpos, ypos);
}

void EngineCoreGLFW::init()
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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
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

    glfwSetWindowSizeCallback(m_mainWindow, EngineCoreGLFW::onWindowResized);

    glfwSwapInterval(0); // disable vsync

#ifdef RENDERER_OPENGL
    m_mainRenderer = new OpenGLRenderer();

    m_mainRenderer->init(VideoMode(m_windowSize.x, m_windowSize.y));
#elif RENDERER_VULKAN
    m_mainRenderer = new VulkanRenderer();

    NativeSurfaceProps surfProps;
#ifdef __linux__
    surfProps.connection = XGetXCBConnection(glfwGetX11Display());
    surfProps.window = glfwGetX11Window(m_mainWindow);
#elif _WIN32
    surfProps.hInstance = GetModuleHandle(NULL);
    surfProps.hwnd = glfwGetWin32Window(m_mainWindow);
#endif

    uint32_t extCount = 0;
    const char **requiredVkExtensions = glfwGetRequiredInstanceExtensions(&extCount); // enable extensions
    std::vector<const char*> instExts(extCount);
    std::copy(requiredVkExtensions, requiredVkExtensions+extCount, instExts.begin());
    ((VulkanRenderer*)m_mainRenderer)->addInstanceExtensions(instExts);

    std::vector<const char*> devExts;
    devExts.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    ((VulkanRenderer*)m_mainRenderer)->addDeviceExtensions(devExts);

#ifndef NDEBUG
    std::vector<const char*> validLayers;
    validLayers.push_back("VK_LAYER_KHRONOS_validation");
    ((VulkanRenderer*)m_mainRenderer)->addValidationLayers(validLayers);
#endif

    ((VulkanRenderer*)m_mainRenderer)->setNSP(surfProps); // native props

    m_mainRenderer->init(VideoMode(m_windowSize.x, m_windowSize.y));
#endif
    ServiceLocator::setRenderer(m_mainRenderer);

    { // events
        glfwSetCursorPosCallback(m_mainWindow, testMouseBind);
    }

    { // UI
#ifdef RENDERER_VULKAN
#ifdef UI_IMGUI
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForVulkan(m_mainWindow, true);
#else
#error Unsupported renderer
#endif
#endif
        UIManager *uimgr = UIManager::create();
        uimgr->init();
//        uimgr->setOnButtonPressedCallback(testOnButtonPressed);
        ServiceLocator::setUIManager(uimgr);
    }

    { // scene
        Scene3D &activeScene = ServiceLocator::getSceneManager().activeScene();
        activeScene.getCamera().setPosition(glm::vec3(0, 2.f, 0));
        activeScene.getCamera().setPitchConstraint(glm::radians(-89.f), glm::radians(89.f));
    }

    m_scriptEngine = new LuaScriptEngine();
    m_scriptEngine->init();
}

void EngineCoreGLFW::terminate()
{
    // TODO: move to ServiceLocator
    delete m_scriptEngine;
    m_scriptEngine = nullptr;
    //
    ServiceLocator::terminate();
    glfwSetInputMode(m_mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwTerminate();
}

void slabContact(Entity *obj, const PhysicsContactData &data)
{
    (void)obj;
    float hitPower = glm::length(data.velocity) * data.mass;
    if(hitPower >= 3.f)
    {
        if(hitPower >= 15.f)
        {
            ServiceLocator::getAudioManager().playSound("crack", SoundPropertiesInfo{1.f, 1.f, data.pos});
        }
        else
            ServiceLocator::getAudioManager().playSound("bounce", SoundPropertiesInfo{1.f, 1.f, data.pos});
    }
}

void dominoContact(Entity *obj, const PhysicsContactData &data)
{
    (void)obj;
    if(glm::length(data.velocity) >= 1.f)
    {
        ServiceLocator::getAudioManager().playSound("click", SoundPropertiesInfo{1.f, 1.f, data.pos});
    }
}
/*
void generateDominoes(Scene3D &currentScene)
{
    ModelManager &mdlMgr = ServiceLocator::getModelManager();
    {
        StaticMesh *floorObj = new StaticMesh();
        floorObj->setModel(mdlMgr.getModel("cube"));
        floorObj->setPosition(glm::vec3(0, 0.f, 0));
        floorObj->setScale(glm::vec3(1000, 1, 1000));
        currentScene.addObject(floorObj);
        // physMgr.createBody(PhysicsBodyCreateInfo{PhysicsBodyShapeInfo{glm::vec3(500.f, .5f, 500.f)}, 0}, floorObj);
    }

    const float pi = glm::pi<float>();

    const Model3D *mdl = mdlMgr.getModel("domino");
    if(mdl == nullptr)
    {
        ServiceLocator::getLogger().error(MODULE_NAME, "'domino' model not found!");
        return;
    }

    const float spawnWidth = 5.f, spawnLength = 50.f;
    const int spawnCount = glm::ceil(spawnLength * 2);

    for(int i=0; i < spawnCount; i++)
    {
        StaticMesh *obj = new StaticMesh();
        obj->setModel(mdl);

        //
        float x = 1.f * i;
        float linePos = spawnWidth * glm::sin(x * (pi / (2.f*spawnWidth)));
        obj->setPosition(glm::vec3(linePos, 0.5f, 1.5f*x));
        obj->setRotation(glm::vec3(0, glm::atan(pi/2.f * glm::cos(x * (pi / (2.f*spawnWidth)))), 0));
        //
        currentScene.addObject(obj);
        physMgr.createBody(PhysicsBodyCreateInfo{PhysicsBodyShapeInfo{glm::vec3(0.62f, 1.17f, .25f)},
                                                 100.f,
                                                 PhysicsBodyProperties{0.6f, 0.2f}}, obj);
    }
}
*/

void EngineCoreGLFW::mainLoop()
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

double EngineCoreGLFW::getDeltaTime() const
{
    return m_deltaTime;
}

double EngineCoreGLFW::getElapsedTime() const
{
    return m_elapsedTime;
}
