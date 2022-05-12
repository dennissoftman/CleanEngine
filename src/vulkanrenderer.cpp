#include "vulkanrenderer.hpp"

#include "vkshader.hpp"

#ifdef __linux__
#include <X11/Xlib-xcb.h>
#elif _WIN32
#include <Windows.h>
#endif

#include <cstring>

#include <vector>
#include <optional>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <source_location>

#include "servicelocator.hpp"

// ---------------------------------------------------------------------------------------------
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator,
                                      VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                           "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    else
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                            "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL customVkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                            void* pUserData)
{
    (void)messageType;
    (void)pUserData;

    if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        fprintf(stderr, "[%s] %s\n", pCallbackData->pMessageIdName, pCallbackData->pMessage);
    return VK_FALSE;
}
// =============================================================================================

const std::string MODULE_NAME = "VulkanRenderer";

VulkanRenderer::VulkanRenderer()
{
}

void VulkanRenderer::init(const VideoMode &mode)
{
    (void)mode;
    ServiceLocator::getLogger().warning(MODULE_NAME, "This renderer is WIP");
}

void VulkanRenderer::queueRenderObject(RenderObject *obj)
{
    (void)obj;
}

void VulkanRenderer::queueRenderObject(VkRenderObject obj)
{
    (void)obj;
}

void VulkanRenderer::draw()
{

}

void VulkanRenderer::setProjectionMatrix(const glm::mat4 &projmx)
{
    (void)projmx;
}

void VulkanRenderer::setViewMatrix(const glm::mat4 &viewmx)
{
    (void)viewmx;
}
