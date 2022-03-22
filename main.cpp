#include "openglrenderer.hpp"
#include "vulkanrenderer.hpp"

#ifdef RENDERER_VULKAN
    #define GLFW_INCLUDE_VULKAN
    #ifdef __linux__
        #define GLFW_EXPOSE_NATIVE_X11
        #include <X11/Xlib-xcb.h>
    #elif _WIN32
        #define GLFW_EXPOSE_NATIVE_WIN32
    #endif
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <cstdio>
#include <string>
#include <stdexcept>

#ifdef RENDERER_OPENGL
#include "glshader.hpp"
#elif RENDERER_VULKAN
#include "vkshader.hpp"
#endif

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
#pragma message("TODO:Logger")
        const char *buf;
        glfwGetError(&buf);
        fprintf(stderr, "%s\n", buf);
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(mainWindow);

    return 0;
}

void keyProcessor(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE)
        glfwSetWindowShouldClose(window, true);
}

void mainLoop()
{
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
    // select hwnd and hinstance
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
#else
#error No graphics API specified
#endif

    glfwSetKeyCallback(mainWindow, keyProcessor);

    while(!glfwWindowShouldClose(mainWindow))
    {
        glfwPollEvents();

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
}

int main()
{
    // init GLFW
    if(glfwInit() != GLFW_TRUE)
    {
#pragma message("TODO:Logger")
        const char *buff;
        glfwGetError(&buff);
        fprintf(stderr, "%s\n", buff);
        throw std::runtime_error("Failed to init GLFW");
    }
    // -----------------------------------------------------------

    initWindow();
    mainLoop();
    cleanup();

    return 0;
}
