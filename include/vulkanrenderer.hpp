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

#endif
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR caps;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanRenderer : public Renderer
{
public:
    VulkanRenderer();
    ~VulkanRenderer();

    void init(const VideoMode &mode) override;

    void draw() override;

    // vulkan-only
    void createSurface();
    void setSurfaceProps(const NativeSurfaceProps &props);

    void addValidationLayer(const char *lay);
    void addInstanceExtension(const char *ext);
    void setRequiredFeatures(VkPhysicalDeviceFeatures feats);
    void addDeviceExtension(const char *ext);

private:
    bool checkValidationLayerSupport();
    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    static SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice &device, const VkSurfaceKHR &surface);
    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, int viewWidth, int viewHeight);

    void recordCommandBuffer(VkCommandBuffer buffer, int imageIdx);

    int m_viewWidth, m_viewHeight;
    std::vector<const char*> m_instanceExtensions, m_deviceExtensions;
    VkPhysicalDeviceFeatures m_requiredFeatures;
#ifndef NDEBUG
    std::vector<const char*> m_validationLayers;
    VkDebugUtilsMessengerEXT m_vkDebugMessenger;
#endif

    VkInstance m_vkInstance;
    VkDevice m_vkDevice;

    VkSurfaceKHR m_vkSurface;
    NativeSurfaceProps m_nativeProps;

    VkQueue m_vkGraphicsQueue, m_vkPresentQueue;

    VkSwapchainKHR m_vkSwapChain;
    std::vector<VkImage> m_vkSwapChainImages;
    VkFormat m_vkSwapChainImageFormat;
    VkExtent2D m_vkSwapChainExtent;

    std::vector<VkImageView> m_vkSwapChainImageViews;

    VkRenderPass m_vkRenderPass;
    VkPipelineLayout m_vkPipelineLayout;
    VkPipeline m_vkGraphicsPipeline;

    std::vector<VkFramebuffer> m_vkSwapChainFramebuffers;

    // TEMP
    VkCommandPool m_vkCommandPool;
    VkCommandBuffer m_vkCommandBuffer;

    VkSemaphore m_vkImageAvailableSemaphore;
    VkSemaphore m_vkRenderFinishedSemaphore;
    VkFence m_vkInFlightFence;
    // =======================================
};

#endif // VULKANRENDERER_HPP
