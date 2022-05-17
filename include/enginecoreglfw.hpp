#ifndef ENGINECOREGLFW_HPP
#define ENGINECOREGLFW_HPP

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

class EngineCoreGLFW
{
public:
    EngineCoreGLFW();
    ~EngineCoreGLFW();

    void init();
    void terminate();

    void mainLoop();

    static void onWindowResized(GLFWwindow *win, int width, int height);
    static EngineCoreGLFW *corePtr;
private:
    GLFWwindow *m_mainWindow;
    Renderer *m_mainRenderer;
    double m_elapsedTime, m_deltaTime;

    glm::ivec2 m_windowSize;
};

#endif // ENGINECOREGLFW_HPP
