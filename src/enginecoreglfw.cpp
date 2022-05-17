#include "enginecoreglfw.hpp"
#include "servicelocator.hpp"

#include <IL/il.h>
#include <IL/ilu.h>

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
    glfwWindowHint(GLFW_DEPTH_BITS, 32); // or fallback to 24
    glfwWindowHint(GLFW_SAMPLES, 2);
    appName = "CleanEngineGL";

#elif RENDERER_VULKAN
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    appName = "CleanEngineVk";

#else
#error No graphics API specified
#endif
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

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
        if(ilLoadImage("data/icons/64.png") == IL_TRUE)
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

        glfwSetWindowIcon(m_mainWindow, icons.size(), icons.data());

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
}

void EngineCoreGLFW::terminate()
{
    ServiceLocator::clear();
    glfwTerminate();
}

void EngineCoreGLFW::mainLoop()
{
    ModelManager &mdlMgr = ServiceLocator::getModelManager();
    mdlMgr.loadModel("data/models/cube.obj", "cube");
    mdlMgr.loadModel("data/models/sphere.obj", "sphere");
    mdlMgr.loadModel("data/models/suzanne.obj", "monkey");

    // TODO: single material for many objects (UBO workaround)
    Material *cubeMat = Material::createMaterial();
    cubeMat->setImage("data/textures/uv.png", "img");
    cubeMat->init();
    mdlMgr.setModelMaterial("cube", cubeMat);

    Material *sphereMat = Material::createMaterial();
    sphereMat->setImage("data/textures/uv.png", "img");
    sphereMat->init();
    mdlMgr.setModelMaterial("sphere", sphereMat);

    Material *monkeyMat = Material::createMaterial();
    monkeyMat->setImage("data/textures/uv.png", "img");
    monkeyMat->init();
    mdlMgr.setModelMaterial("monkey", monkeyMat);
    // =============================================================================================

    StaticMesh *cubeObj = new StaticMesh();
    cubeObj->setModel(mdlMgr.getModel("cube"));
    StaticMesh *sphereObj = new StaticMesh();
    sphereObj->setModel(mdlMgr.getModel("sphere"));
    StaticMesh *monkeyObj = new StaticMesh();
    monkeyObj->setModel(mdlMgr.getModel("monkey"));

    Scene3D *currentScene = new Scene3D();
    cubeObj->setPos(glm::vec3(-2, 0, 0));
    currentScene->addObject(cubeObj);
    sphereObj->setPos(glm::vec3(0, 0, 0));
    currentScene->addObject(sphereObj);
    monkeyObj->setPos(glm::vec3(2, 0, 0));
    currentScene->addObject(monkeyObj);

    // "Camera" init
    glm::mat4 _projMatrix, _viewMatrix;
    _projMatrix = glm::perspective(90.f, (float)m_windowSize.x/(float)m_windowSize.y, 0.1f, 1000.f);
    _viewMatrix = glm::lookAt(
        glm::vec3(0, 1.5f, -1),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0));

    m_mainRenderer->setViewMatrix(_viewMatrix);
    m_mainRenderer->setProjectionMatrix(_projMatrix);
    //

    while (!glfwWindowShouldClose(m_mainWindow))
    {
        m_deltaTime = glfwGetTime() - m_elapsedTime;
        m_elapsedTime = glfwGetTime();
        glfwPollEvents();

        cubeObj->setRotation(glm::vec3(0, m_elapsedTime/2, 0));
        sphereObj->setRotation(glm::vec3(0, m_elapsedTime/2, 0));
        monkeyObj->setRotation(glm::vec3(0, m_elapsedTime/2, 0));

        // Draw objects on screen
        currentScene->draw(m_mainRenderer);

        m_mainRenderer->draw();

        // =====================================================================================
#ifdef RENDERER_OPENGL
        glfwSwapBuffers(m_mainWindow);
#endif
    }
    delete currentScene;
}
