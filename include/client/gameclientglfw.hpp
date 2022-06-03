#ifndef GAMECLIENTGLFW_H
#define GAMECLIENTGLFW_H

#ifdef RENDERER_VULKAN
    #define GLFW_INCLUDE_VULKAN
    #ifdef __linux__
        #define GLFW_EXPOSE_NATIVE_X11
        #include <X11/Xlib-xcb.h>
    #elif _WIN32
        #define GLFW_EXPOSE_NATIVE_WIN32
    #endif
    #include "client/vulkanrenderer.hpp"
#elif RENDERER_OPENGL
    #include "openglrenderer.hpp"
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

class ScriptEngine;

class GameClientGLFW
{
public:
    GameClientGLFW();
    ~GameClientGLFW();

    void init();
    void terminate();

    void mainLoop();

    double getDeltaTime() const;
    double getElapsedTime() const;

    GLFWwindow *getWindowPtr() const;

    static void onWindowResized(GLFWwindow *win, int width, int height);
    static GameClientGLFW *corePtr;
private:
    GLFWwindow *m_mainWindow;
    Renderer *m_mainRenderer;
    double m_elapsedTime, m_deltaTime;
    // TEMP
    ScriptEngine *m_scriptEngine;

    glm::ivec2 m_windowSize;
};

#endif // GAMECLIENTGLFW_H