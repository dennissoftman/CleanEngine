#ifndef VULKANRENDERER_HPP
#define VULKANRENDERER_HPP

#include "renderer.hpp"

#ifdef __linux__
#define VK_USE_PLATFORM_XCB_KHR
#elif _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <vulkan/vulkan.h>

#include <vector>
#include <optional>

struct NativeSurfaceProps
{
#ifdef __linux__
    std::optional<xcb_connection_t*> connection;
    std::optional<xcb_window_t> window;

    bool isComplete() const
    {
        return connection.has_value() && window.has_value();
    }
#elif _WIN32
    std::optional<HWND> hwnd;
    std::optional<HINSTANCE> hInstance;

    bool isComplete() const
    {
        return hwnd.has_value() && hInstance.has_value();
    }
#endif
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR caps;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct VkRenderObject
{
    RenderObject *parent;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
};

struct VkShaderTransformData
{
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;
};

class VulkanRenderer : public Renderer
{
public:
    VulkanRenderer();
    ~VulkanRenderer();

    void init(const VideoMode &mode) override;

    void queueRenderObject(RenderObject *obj) override;
    void queueRenderObject(VkRenderObject obj);
    void draw() override;

    void setProjectionMatrix(const glm::mat4 &projmx) override;
    void setViewMatrix(const glm::mat4 &viewmx) override;
private:

};

#endif // VULKANRENDERER_HPP
