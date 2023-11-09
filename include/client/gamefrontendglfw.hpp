#ifndef GAMECLIENTGLFW_H
#define GAMECLIENTGLFW_H

#include "client/gamefrontend.hpp"

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

#include "server/scene3d.hpp"

class GameFrontendGLFW : public GameFrontend
{
public:
    GameFrontendGLFW();
    ~GameFrontendGLFW();

    void init() override;
    void run() override;
    void terminate() override;

    void mainLoop();

    Scene3D &getScene() override;

    double getDeltaTime() const override;
    double getElapsedTime() const override;

    void lockCursor() override;
    void unlockCursor() override;

    GLFWwindow *getWindowPtr() const;

    void updateSubscribe(const std::function<void(double)> &callb) override;

    // for events
    static void onKeyboardEvent(GLFWwindow *win, int key, int scancode, int action, int mods);
    static void onMouseButtonEvent(GLFWwindow *win, int button, int action, int mods);
    static void onMousePositionEvent(GLFWwindow *win, double mx, double my);
    static void onMouseScrollEvent(GLFWwindow *win, double sx, double sy);
    static void onJoystickEvent(int jid, int event);

    static void onWindowResized(GLFWwindow *win, int width, int height);
    //
    static GameFrontendGLFW *corePtr;
private:
    GLFWwindow *m_mainWindow;
    Renderer *m_mainRenderer;
    Scene3D m_currentScene;
    boost::signals2::signal<void(double)> m_updateEvents;
    double m_elapsedTime, m_deltaTime;

    // window options
    glm::ivec2 m_windowSize;
    bool m_windowFullscreen, m_windowVSync;
    uint32_t m_windowFpsCap, m_windowSamples;
};

#endif // GAMECLIENTGLFW_H
