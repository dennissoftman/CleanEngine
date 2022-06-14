#ifndef GAMECLIENTGLFW_H
#define GAMECLIENTGLFW_H

#include "client/gameclient.hpp"

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

class GameClientGLFW : public GameClient
{
public:
    GameClientGLFW();
    ~GameClientGLFW();

    void init() override;
    void run() override;
    void terminate() override;

    void mainLoop();

    double getDeltaTime() const override;
    double getElapsedTime() const override;

    void lockCursor() override;
    void unlockCursor() override;

    GLFWwindow *getWindowPtr() const;

    // for events
    static void onKeyboardEvent(GLFWwindow *win, int key, int scancode, int action, int mods);
    static void onMouseButtonEvent(GLFWwindow *win, int button, int action, int mods);
    static void onMousePositionEvent(GLFWwindow *win, double mx, double my);
    static void onMouseScrollEvent(GLFWwindow *win, double sx, double sy);

    static void onWindowResized(GLFWwindow *win, int width, int height);
    //
    static GameClientGLFW *corePtr;
private:
    GLFWwindow *m_mainWindow;
    Renderer *m_mainRenderer;
    double m_elapsedTime, m_deltaTime;

    glm::ivec2 m_windowSize;
};

#endif // GAMECLIENTGLFW_H
