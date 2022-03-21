#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vector>

#include <cstdio>

#include "version.h"

static GLFWwindow *mainWindow = nullptr;
static int scrWidth = 1280, scrHeight = 720;

int initWindow()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mainWindow = glfwCreateWindow(scrWidth, scrHeight, "CleanEngine", nullptr, nullptr);

    return 0;
}

void mainLoop()
{
    while(!glfwWindowShouldClose(mainWindow))
    {
        glfwPollEvents();

        glfwSwapBuffers(mainWindow);
    }
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
        const char *buff;
        glfwGetError(&buff);
        fprintf(stderr, "Failed to init GLFW\nWhat: %s\n", buff);
    }
    // -----------------------------------------------------------

    initWindow();
    mainLoop();
    cleanup();

    return 0;
}
