#include "enginecoreglfw.hpp"
#include "servicelocator.hpp"

#include <IL/il.h>
#include <IL/ilu.h>

#include "luascriptengine.hpp"

EngineCoreGLFW *EngineCoreGLFW::corePtr = nullptr;
static const char *MODULE_NAME = "EngineCoreGLFW";

EngineCoreGLFW::EngineCoreGLFW()
    : m_mainWindow(nullptr),
      m_elapsedTime(0), m_deltaTime(0),
      m_windowSize(glm::ivec2(1280, 720))
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

    // Init DevIL
    {
        ilInit();
        ILuint r;
        if ((r = ilGetError()) == IL_NO_ERROR)
            ServiceLocator::getLogger().info(MODULE_NAME, "DevIL init OK");
        else
        {
            std::stringstream errstr;
            errstr << "Failed to init DevIL: " << std::hex << r;
            ServiceLocator::getLogger().error(MODULE_NAME, errstr.str());
        }
        iluInit();
    }
    // ==========

    // icons
    {
        std::vector<GLFWimage> icons;
        ILuint img = ilGenImage();
        ilBindImage(img);
        if(ilLoadImage(ServiceLocator::getResourceManager().getEnginePath("data/icons/64.png").c_str()) == IL_TRUE)
        {
            GLFWimage icon;
            icon.width = ilGetInteger(IL_IMAGE_WIDTH);
            icon.height = ilGetInteger(IL_IMAGE_HEIGHT);
            icon.pixels = ilGetData();
            icons.push_back(icon);
        }
        else
        {
            std::stringstream errstr;
            errstr << "Failed to load icon: " << iluErrorString(ilGetError());
            ServiceLocator::getLogger().error(MODULE_NAME, errstr.str());
        }
        ilBindImage(0);

        glfwSetWindowIcon(m_mainWindow, static_cast<int>(icons.size()), icons.data());

        ilDeleteImage(img);
    }
    // =====

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

    {
        Scene3D mainScene;
        mainScene.getCamera().setPosition(glm::vec3(0, 2.f, 0));
        mainScene.getCamera().setPitchConstraint(glm::radians(-89.f), glm::radians(89.f));
        ServiceLocator::getSceneManager().addScene(std::move(mainScene), "main");
    }

    m_scriptEngine = new LuaScriptEngine();
    m_scriptEngine->init();
}

void EngineCoreGLFW::terminate()
{
    ServiceLocator::clear();
    glfwSetInputMode(m_mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwTerminate();
}

void testKeyBind(GLFWwindow *win, int key, int scancode, int action, int mods)
{
    (void)win;
    (void)scancode;
    (void)action;
    (void)mods;

    if(key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(win, GLFW_TRUE);
}

static glm::ivec2 OldCursorPos = glm::ivec2(0,0);

void testMouseButtonBind(GLFWwindow *win, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        {
            Scene3D *currentScene = ServiceLocator::getSceneManager().getScene("main");
            Camera3D &cam = currentScene->getCamera();

            StaticMesh *sphereObj = new StaticMesh();
            sphereObj->setModel(ServiceLocator::getModelManager().getModel("sphere"));
            sphereObj->setPosition(cam.getPosition() + 2.f*cam.frontVector());
            sphereObj->setScale(glm::vec3(2.f, 2.f, 2.f));
            currentScene->addObject(sphereObj);
            ServiceLocator::getPhysicsManager().createBody(PhysicsBodyCreateInfo{PhysicsBodyShapeInfo{1.f}, 5.f, PhysicsBodyProperties{0.f}, 200.f*cam.frontVector()}, sphereObj);
        }
    }
}

void testMouseBind(GLFWwindow *win, double xpos, double ypos)
{
    (void)win;

    {
        Camera3D &cam = ServiceLocator::getSceneManager().getScene("main")->getCamera();
        float sens = 3.f;
        float deltaTime = EngineCoreGLFW::corePtr->getDeltaTime();

        cam.rotate(sens * deltaTime * (ypos - OldCursorPos.y), glm::vec3(-1, 0, 0));
        cam.rotate(sens * deltaTime * (xpos - OldCursorPos.x), glm::vec3(0, 1, 0));
    }

    OldCursorPos = glm::ivec2(xpos, ypos);
}

void EngineCoreGLFW::mainLoop()
{
    Scene3D *currentScene = ServiceLocator::getSceneManager().getScene("main");

    // TODO: scripts and VkMaterial fix (UBO)
    ModelManager &mdlMgr = ServiceLocator::getModelManager();

    {
        PhysicsManager &physMgr = ServiceLocator::getPhysicsManager();
        // static floor
        {
            StaticMesh *floorObj = new StaticMesh();
            floorObj->setModel(mdlMgr.getModel("cube"));
            floorObj->setPosition(glm::vec3(0, 0.f, 0));
            floorObj->setScale(glm::vec3(100, 1, 100));
            currentScene->addObject(floorObj);
            physMgr.createBody(PhysicsBodyCreateInfo{PhysicsBodyShapeInfo{glm::vec3(50.f, .5f, 50.f)}, 0}, floorObj);
        }

        //

        const int spawnRadius = 32, spawnHeight = 16;
        const float circleLen = 2.f*M_PI*spawnRadius;
        const int cubesInRing = ceil(circleLen*0.45f);
        for(int i=0; i < spawnHeight; i++)
        {
            for(int j=0; j < cubesInRing; j++)
            {
                StaticMesh *obj = new StaticMesh();
                obj->setModel(mdlMgr.getModel("cube"));
                obj->setScale(glm::vec3(2.f, 1.f, 1.f));

                float circlePos =  ((float)j-(i%2)*0.5f)/(float)cubesInRing;

                obj->setPosition(glm::vec3(sinf(circlePos * 2.f * M_PIf) * spawnRadius,
                                           i+0.5f,
                                           cosf(circlePos * 2.f * M_PIf) * spawnRadius)
                                 );
                obj->setRotation(glm::vec3(0, circlePos * 2.f * M_PIf, 0));
                currentScene->addObject(obj);
                physMgr.createBody(PhysicsBodyCreateInfo{PhysicsBodyShapeInfo{glm::vec3(1.f, .5f, .5f)}, 2.f}, obj);
            }
        }
    }

    glfwSetKeyCallback(m_mainWindow, testKeyBind);
    glfwSetMouseButtonCallback(m_mainWindow, testMouseButtonBind);
    glfwSetCursorPosCallback(m_mainWindow, testMouseBind);
    glfwSetInputMode(m_mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    {
        double xpos, ypos;
        glfwGetCursorPos(m_mainWindow, &xpos, &ypos);
        OldCursorPos = glm::ivec2(xpos, ypos);
    }

    // =========================================================================================

    PhysicsManager &physicsManager = ServiceLocator::getPhysicsManager();
    while (!glfwWindowShouldClose(m_mainWindow))
    {
        m_deltaTime = glfwGetTime() - m_elapsedTime;
        m_elapsedTime = glfwGetTime();
        glfwPollEvents();

        // update physics
        physicsManager.update(m_deltaTime);
        // update camera
        {
            Camera3D &cam = currentScene->getCamera();
            float cam_speed = 10.f;
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
        currentScene->update(m_deltaTime);

        // Draw objects on screen
        currentScene->draw(m_mainRenderer);

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
