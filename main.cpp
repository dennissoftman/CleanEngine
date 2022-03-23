#ifdef RENDERER_VULKAN
    #define GLFW_INCLUDE_VULKAN
    #ifdef __linux__
        #define GLFW_EXPOSE_NATIVE_X11
        #include <X11/Xlib-xcb.h>
    #elif _WIN32
        #define GLFW_EXPOSE_NATIVE_WIN32
    #endif
    #include "vulkanrenderer.hpp"
#elif RENDERER_OPENGL
    #include "openglrenderer.hpp"
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <string>
#include <stdexcept>
#include <cstdio>

#include "servicelocator.hpp"

#ifndef NDEBUG
#include "debuglogger.hpp"
#endif

#ifdef RENDERER_OPENGL
#include "glshader.hpp"
#elif RENDERER_VULKAN
#include "vkshader.hpp"
#endif

#include <glm/gtx/transform.hpp>

const std::string MODULE_NAME = "Main";

static GLFWwindow *mainWindow = nullptr;
static int scrWidth = 1280, scrHeight = 720;

int initWindow()
{
    std::string appName;
#ifdef RENDERER_OPENGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 2);
    appName = "CleanEngineGL";
#elif RENDERER_VULKAN
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    appName = "CleanEngineVk";
#else
#error No graphics API specified
#endif

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mainWindow = glfwCreateWindow(scrWidth, scrHeight, appName.c_str(), nullptr, nullptr);
    if(mainWindow == NULL)
    {
        const char *buff;
        glfwGetError(&buff);
        ServiceLocator::getLogger().info(MODULE_NAME, buff);
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(mainWindow);

    glfwSwapInterval(0);

    return 0;
}

void keyProcessor(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)window;
    (void)scancode;
    (void)action;
    (void)mods;

    if(key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, true);
}

void mainLoop()
{
    ServiceLocator::init();
    {
//        FILE *debugFP = fopen("debug.log", "a");
        DebugLogger *logger = new DebugLogger();
//        logger->setInfoFP(debugFP);
//        logger->setWarningFP(debugFP);
//        logger->setErrorFP(debugFP);
        ServiceLocator::setLogger(logger);
    }
    ServiceLocator::getLogger().info(MODULE_NAME, "Started logging");

    Renderer *rend = nullptr;
#ifdef RENDERER_OPENGL
    rend = new OpenGLRenderer();

    rend->init(VideoMode(scrWidth, scrHeight));

    GLShader mainShader;
    mainShader.load("data/shaders/gl/main.vert", "data/shaders/gl/main.frag");

    ((OpenGLRenderer*)rend)->setShader(mainShader);
#elif RENDERER_VULKAN
    rend = new VulkanRenderer();

    NativeSurfaceProps props;
#ifdef __linux__
    props.connection = XGetXCBConnection(glfwGetX11Display());
    if(props.connection.value() == NULL)
        throw std::runtime_error("XCB connection is NULL");
    props.window = glfwGetX11Window(mainWindow);
    if(props.window.value() == None)
        throw std::runtime_error("XCB window is NULL");
#elif _WIN32
    props.hwnd = glfwGetWin32Window(mainWindow);
    props.hInstance = GetModuleHandle(NULL);
#endif

#ifndef NDEBUG
    ((VulkanRenderer*)rend)->addInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    ((VulkanRenderer*)rend)->addValidationLayer("VK_LAYER_KHRONOS_validation");
#endif

    ((VulkanRenderer*)rend)->setSurfaceProps(NativeSurfaceProps(props));

    uint32_t requiredExtCount = 0;
    const char **requiredExts = glfwGetRequiredInstanceExtensions(&requiredExtCount);
    for(uint32_t i=0; i < requiredExtCount; i++)
        ((VulkanRenderer*)rend)->addInstanceExtension(requiredExts[i]);

    ((VulkanRenderer*)rend)->addDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    rend->init(VideoMode(scrWidth, scrHeight));

    VkShader mainShader;
#else
#error No graphics API specified
#endif

    glfwSetKeyCallback(mainWindow, keyProcessor);

    Model2D *triangle = new Model2D(
                {
                    {
                        glm::vec2(-0.5f, -0.5f), glm::vec3(1.f, 0.f, 0.f),
                        glm::vec2(+0.0f, +0.5f), glm::vec3(0.f, 1.f, 0.f),
                        glm::vec2(+0.5f, -0.5f), glm::vec3(0.f, 0.f, 1.f)
                    }
                });

    std::vector<RenderObject> triangles;

    struct TriangleData {
        bool vx, vy;
        glm::vec2 pos;
    };

    for(int i=0; i < 100; i++)
    {
        TriangleData *triData = new TriangleData;
        triData->vx = (rand()%100) > 50 ? true : false;
        triData->vy = (rand()%100) > 50 ? true : false;
        triData->pos = glm::vec2((rand() % 100)/10.f - 5.f, (rand() % 100)/10.f - 5.f);

        RenderObject obj;
        obj.model = triangle;
        obj.shader = &mainShader;
        obj.pUserData = triData;
        triangles.push_back(std::move(obj));
    }

    double lastTime, dt;
    while(!glfwWindowShouldClose(mainWindow))
    {
        dt = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();
        glfwPollEvents();

        //
        for(RenderObject &obj : triangles)
        {
            TriangleData &triData = *(TriangleData*)obj.pUserData;
            if(triData.vy)
            {
                if(triData.pos.y <= -5.f)
                    triData.vy = false;
                else
                    triData.pos.y -= dt;
            }
            else
            {
                if(triData.pos.y >= 5.f)
                    triData.vy = true;
                else
                    triData.pos.y += dt;
            }

            if(triData.vx)
            {
                if(triData.pos.x >= 5.f)
                    triData.vx = false;
                else
                    triData.pos.x += dt;
            }
            else
            {
                if(triData.pos.x <= -5.f)
                    triData.vx = true;
                else
                    triData.pos.x -= dt;
            }
            obj.modelMatrix = glm::scale(glm::mat4(1.f), glm::vec3(0.25f));
            obj.modelMatrix = glm::rotate(obj.modelMatrix, triData.pos.x * triData.pos.y, glm::vec3(0, 0, 1));
            obj.modelMatrix = glm::translate(obj.modelMatrix, glm::vec3(triData.pos, 0));
            rend->queueRenderObject(&obj);
        }
        //

        rend->draw();
#ifdef RENDERER_OPENGL
        glfwSwapBuffers(mainWindow);
#endif
    }

    delete rend;
}

void cleanup()
{
    glfwTerminate();
    ServiceLocator::clear();
}

int main()
{
    // init GLFW
    if(glfwInit() != GLFW_TRUE)
    {
        const char *buff;
        glfwGetError(&buff);
        ServiceLocator::getLogger().info(MODULE_NAME, buff);
        throw std::runtime_error("Failed to init GLFW");
    }
    // -----------------------------------------------------------

    initWindow();
    mainLoop();
    cleanup();

    return 0;
}
