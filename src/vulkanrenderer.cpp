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
                                   const VkAllocationCallbacks* pAllocator) {
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
    : m_requiredFeatures({}),
      m_vkInstance(VK_NULL_HANDLE),
      m_vkDevice(VK_NULL_HANDLE)
{

}

VulkanRenderer::~VulkanRenderer()
{
    vkDeviceWaitIdle(m_vkDevice);

    //
    for(const VkRenderObject &obj : m_createdObjects)
    {
        vkFreeMemory(m_vkDevice, obj.vertexBufferMemory, nullptr);
        vkDestroyBuffer(m_vkDevice, obj.vertexBuffer, nullptr);
    }
    //

    vkDestroyFence(m_vkDevice, m_vkInFlightFence, nullptr);
    vkDestroySemaphore(m_vkDevice, m_vkRenderFinishedSemaphore, nullptr);
    vkDestroySemaphore(m_vkDevice, m_vkImageAvailableSemaphore, nullptr);

    vkDestroyCommandPool(m_vkDevice, m_vkCommandPool, nullptr);

    for(auto &fbuff : m_vkSwapChainFramebuffers)
    {
        vkDestroyFramebuffer(m_vkDevice, fbuff, nullptr);
    }

    vkDestroyPipeline(m_vkDevice, m_vkGraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(m_vkDevice, m_vkPipelineLayout, nullptr);
    vkDestroyRenderPass(m_vkDevice, m_vkRenderPass, nullptr);

    for(auto &imageView : m_vkSwapChainImageViews)
    {
        vkDestroyImageView(m_vkDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(m_vkDevice, m_vkSwapChain, nullptr);

    vkDestroyDevice(m_vkDevice, nullptr);

    vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);

#ifndef NDEBUG
    DestroyDebugUtilsMessengerEXT(m_vkInstance, m_vkDebugMessenger, nullptr);
#endif

    vkDestroyInstance(m_vkInstance, nullptr);
}

bool VulkanRenderer::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : m_validationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

void VulkanRenderer::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = customVkDebugCallback;
    createInfo.pUserData = nullptr;
}

void VulkanRenderer::init(const VideoMode &mode)
{
    Logger &logger = ServiceLocator::getLogger();

    m_viewWidth = mode.width;
    m_viewHeight = mode.height;

    // Create instance
    {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "CleanEngine";
        appInfo.applicationVersion = 1;
        appInfo.apiVersion = VK_API_VERSION_1_1;

        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &appInfo;
#ifndef NDEBUG
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if(checkValidationLayerSupport())
        {
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = m_validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            instanceCreateInfo.pNext = &debugCreateInfo;
        }
#endif
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_instanceExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = m_instanceExtensions.data();

        logger.info(MODULE_NAME, "Creating VkInstance with extensions:");
        for(auto &ext : m_instanceExtensions)
            logger.info(MODULE_NAME, "\t"+std::string(ext));

        if(vkCreateInstance(&instanceCreateInfo, nullptr, &m_vkInstance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create VkInstance");
        }
    }
    // =========================================================================================

    // Create debugger

    {
#ifndef NDEBUG
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        populateDebugMessengerCreateInfo(debugCreateInfo);
        if(CreateDebugUtilsMessengerEXT(m_vkInstance, &debugCreateInfo, nullptr, &m_vkDebugMessenger) != VK_SUCCESS)
        {
            ServiceLocator::getLogger().info(__PRETTY_FUNCTION__, "Failed to create VkDebugUtilsMessenger");
        }
#endif
    }

    // =========================================================================================

    // Create surface
    createSurface();

    // Select physical device
    VkPhysicalDevice targetPDev = VK_NULL_HANDLE;
    uint32_t pDevCount = 0;
    vkEnumeratePhysicalDevices(m_vkInstance, &pDevCount, nullptr);
    std::vector<VkPhysicalDevice> pDevs(pDevCount);
    vkEnumeratePhysicalDevices(m_vkInstance, &pDevCount, pDevs.data());

    logger.info(MODULE_NAME, "Detected devices:");
    for(const VkPhysicalDevice &pDev : pDevs)
    {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(pDev, &props);
        char buff[256];
        sprintf(buff,
                "\t%04X:%04X %s",
                props.vendorID, props.deviceID,
                props.deviceName);
        logger.info(MODULE_NAME, buff);

        VkPhysicalDeviceFeatures feats;
        vkGetPhysicalDeviceFeatures(pDev, &feats);

        if(feats.geometryShader) // we need this to draw
        {
            targetPDev = pDev;
            break;
        }
    }

    if(targetPDev == VK_NULL_HANDLE)
    {
        logger.error(MODULE_NAME, "Failed to find suitable vulkan device");
        throw std::runtime_error("Failed to find suitable vulkan device");
    }
    m_vkPhysicalDevice = targetPDev;

    // Create logical device
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProps(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &queueFamilyCount, queueFamilyProps.data());

    struct QueueFamilyIndicies
    {
        std::optional<uint32_t> graphicsQueueID;
        std::optional<uint32_t> presentationQueueID;

        bool isComplete() const
        {
            return graphicsQueueID.has_value() && presentationQueueID.has_value();
        }
    } queueIdx;

    for(uint32_t i=0; VkQueueFamilyProperties prop : queueFamilyProps)
    {
        if(prop.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            queueIdx.graphicsQueueID = i;

        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(m_vkPhysicalDevice, i, m_vkSurface, &presentSupport);

        if(presentSupport)
            queueIdx.presentationQueueID = i;

        if(queueIdx.isComplete())
            break;

        i++;
    }

    if(!queueIdx.isComplete())
    {
        logger.error(MODULE_NAME, "Failed to find device with graphics and presentation queues");
        throw std::runtime_error("Failed to find device with graphics and presentation queues");
    }

    float queuePriorities = 1.f;

    VkDeviceQueueCreateInfo graphicsQueueCreateInfo{};
    graphicsQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicsQueueCreateInfo.pQueuePriorities = &queuePriorities;
    graphicsQueueCreateInfo.queueCount = 1;
    graphicsQueueCreateInfo.queueFamilyIndex = queueIdx.graphicsQueueID.value();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos = { graphicsQueueCreateInfo };

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &m_requiredFeatures;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

    logger.info(MODULE_NAME, "Creating VkDevice with extensions:");
    for(auto &ext : m_deviceExtensions)
        logger.info(MODULE_NAME, "\t"+std::string(ext));

    if(vkCreateDevice(m_vkPhysicalDevice, &deviceCreateInfo, nullptr, &m_vkDevice) != VK_SUCCESS)
    {
        logger.error(MODULE_NAME, "Failed to create VkDevice");
        throw std::runtime_error("Failed to create VkDevice");
    }

    // Get queues
    vkGetDeviceQueue(m_vkDevice, queueIdx.graphicsQueueID.value(), 0, &m_vkGraphicsQueue);
    vkGetDeviceQueue(m_vkDevice, queueIdx.presentationQueueID.value(), 0, &m_vkPresentQueue);

    // Create swapchain
    {
        SwapChainSupportDetails swapChainSupport;

        swapChainSupport = querySwapChainSupport(m_vkPhysicalDevice, m_vkSurface);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.caps, m_viewWidth, m_viewHeight);

        uint32_t imageCount = swapChainSupport.caps.minImageCount + 1;
        if (swapChainSupport.caps.maxImageCount > 0 && imageCount > swapChainSupport.caps.maxImageCount) {
            imageCount = swapChainSupport.caps.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_vkSurface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queueFamilyIndices[] = {queueIdx.graphicsQueueID.value(),
                                         queueIdx.presentationQueueID.value()};

        if (queueIdx.graphicsQueueID != queueIdx.presentationQueueID)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        } else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.caps.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_vkDevice, &createInfo, nullptr, &m_vkSwapChain) != VK_SUCCESS)
        {
            logger.error(MODULE_NAME, "Failed to create VkSwapChainKHR");
            throw std::runtime_error("Failed to create VkSwapChainKHR");
        }

        vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapChain, &imageCount, nullptr);
        m_vkSwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapChain, &imageCount, m_vkSwapChainImages.data());

        m_vkSwapChainImageFormat = surfaceFormat.format;
        m_vkSwapChainExtent = extent;
    }
    // =========================================================================================

    // Create swapChainImageViews
    {
        m_vkSwapChainImageViews.resize(m_vkSwapChainImages.size());

        for (size_t i = 0; i < m_vkSwapChainImageViews.size(); i++)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_vkSwapChainImages[i];

            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_vkSwapChainImageFormat;

            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if(vkCreateImageView(m_vkDevice, &createInfo, nullptr, &m_vkSwapChainImageViews[i]) != VK_SUCCESS)
            {
                logger.error(MODULE_NAME, "Failed to create image view");
                throw std::runtime_error("Failed to create image view");
            }
        }
    }
    // =========================================================================================

    // Create render pass
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_vkSwapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(m_vkDevice, &renderPassInfo, nullptr, &m_vkRenderPass) != VK_SUCCESS)
        {
            logger.error(MODULE_NAME, "Failed to create render pass");
            throw std::runtime_error("Failed to create render pass");
        }
    }
    // =========================================================================================

    // Create shaders and pipeline
#pragma message("TODO:Independent shaders")
    {
        VkShader mainShader;
        mainShader.setDevice(m_vkDevice);
        mainShader.load("data/shaders/vk/main.vert.spv", "data/shaders/vk/main.frag.spv");

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

        vertShaderStageInfo.module = ((VkShader*)&mainShader)->getVertexModule();
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = ((VkShader*)&mainShader)->getFragmentModule();
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        //
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(glm::vec2) + sizeof(glm::vec3);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = 0;

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = sizeof(glm::vec2);
        //

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = m_vkSwapChainExtent.height;
        viewport.width = (float)m_vkSwapChainExtent.width;
        viewport.height = -(float)m_vkSwapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m_vkSwapChainExtent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                              VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT |
                                              VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional

        /*
        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();
        */

        VkPushConstantRange pushConstant;
        pushConstant.offset = 0;
        pushConstant.size = sizeof(glm::mat4);
        pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstant;

        if(vkCreatePipelineLayout(m_vkDevice, &pipelineLayoutInfo, nullptr, &m_vkPipelineLayout) != VK_SUCCESS)
        {
            logger.error(MODULE_NAME, "Failed to create pipeline layout");
            throw std::runtime_error("Failed to create pipeline layout");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = nullptr; // Optional
        pipelineInfo.layout = m_vkPipelineLayout;
        pipelineInfo.renderPass = m_vkRenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional

        if(vkCreateGraphicsPipelines(m_vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_vkGraphicsPipeline) != VK_SUCCESS)
        {
            logger.error(MODULE_NAME, "Failed to create graphics pipeline");
            throw std::runtime_error("Failed to create graphics pipeline");
        }
    }
    // =========================================================================================

    // Create framebuffers
    {
        m_vkSwapChainFramebuffers.resize(m_vkSwapChainImageViews.size());

        for(size_t i = 0; i < m_vkSwapChainFramebuffers.size(); i++)
        {
            VkImageView attachments[] = {
                m_vkSwapChainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_vkRenderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = m_vkSwapChainExtent.width;
            framebufferInfo.height = m_vkSwapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_vkDevice, &framebufferInfo, nullptr, &m_vkSwapChainFramebuffers[i]) != VK_SUCCESS)
            {
                logger.error(MODULE_NAME, "Failed to create framebuffer");
                throw std::runtime_error("Failed to create framebuffer");
            }
        }
    }
    // =========================================================================================

    // Create command pool
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueIdx.graphicsQueueID.value();

        if(vkCreateCommandPool(m_vkDevice, &poolInfo, nullptr, &m_vkCommandPool) != VK_SUCCESS)
        {
            logger.error(MODULE_NAME, "Failed to create command pool");
            throw std::runtime_error("Failed to create command pool");
        }

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_vkCommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(m_vkDevice, &allocInfo, &m_vkCommandBuffer) != VK_SUCCESS)
        {
            logger.error(MODULE_NAME, "Failed to allocate command buffers");
            throw std::runtime_error("Failed to allocate command buffers");
        }
    }
    // =========================================================================================

    // Sync objects
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if(vkCreateSemaphore(m_vkDevice, &semaphoreInfo, nullptr, &m_vkImageAvailableSemaphore) != VK_SUCCESS ||
           vkCreateSemaphore(m_vkDevice, &semaphoreInfo, nullptr, &m_vkRenderFinishedSemaphore) != VK_SUCCESS ||
           vkCreateFence(m_vkDevice, &fenceInfo, nullptr, &m_vkInFlightFence) != VK_SUCCESS)
        {
            logger.error(MODULE_NAME, "Failed to create sync objects");
            throw std::runtime_error("Failed to create sync objects");
        }
    }
    // =========================================================================================
}

void VulkanRenderer::queueRenderObject(RenderObject *obj)
{
    for(const VkRenderObject &vObj : m_createdObjects)
    {
        if(vObj.parent == obj) // do not create duplicates
        {
            queueRenderObject(vObj);
            return;
        }
    }
    VkRenderObject vObj = createRenderObject(obj);
    m_createdObjects.push_back(vObj);
    queueRenderObject(vObj);
}

void VulkanRenderer::queueRenderObject(VkRenderObject obj)
{
    m_renderQueue.push(obj);
}

VkRenderObject VulkanRenderer::createRenderObject(RenderObject *obj)
{
    VkRenderObject vObj;
    vObj.parent = obj;
    //
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = obj->model->tris.size() * sizeof(Triangle2D);
    bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if(vkCreateBuffer(m_vkDevice, &bufferInfo, nullptr, &vObj.vertexBuffer) != VK_SUCCESS)
    {
        ServiceLocator::getLogger().error(MODULE_NAME, "Failed to create VkBuffer");
        throw std::runtime_error("Failed to create VkBuffer");
    }

    // Check for memory, meh
    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(m_vkDevice, vObj.vertexBuffer, &memReqs);

    // Allocate memory
    VkDeviceMemory vertexBufferMemory;
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;
    allocInfo.memoryTypeIndex = findMemoryType(m_vkPhysicalDevice,
                                               memReqs.memoryTypeBits,
                                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                               VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    if(vkAllocateMemory(m_vkDevice, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS)
    {
        ServiceLocator::getLogger().error(MODULE_NAME, "Failed to allocate memory for vertex buffer");
        throw std::runtime_error("Failed to allocate memory for vertex buffer");
    }
    vkBindBufferMemory(m_vkDevice, vObj.vertexBuffer, vertexBufferMemory, 0);

    // Map memory
    void *data;
    vkMapMemory(m_vkDevice, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
    memcpy(data, obj->model->tris.data(), obj->model->tris.size() * sizeof(Triangle2D));
    vkUnmapMemory(m_vkDevice, vertexBufferMemory);

    return vObj;
}

SwapChainSupportDetails VulkanRenderer::querySwapChainSupport(const VkPhysicalDevice &device, const VkSurfaceKHR &surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.caps);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

VkSurfaceFormatKHR VulkanRenderer::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR VulkanRenderer::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int viewWidth, int viewHeight)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D actualExtent =
        {
            static_cast<uint32_t>(viewWidth),
            static_cast<uint32_t>(viewHeight)
        };

        actualExtent.width = std::clamp(actualExtent.width,
                                        capabilities.minImageExtent.width,
                                        capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height,
                                         capabilities.minImageExtent.height,
                                         capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

uint32_t VulkanRenderer::findMemoryType(VkPhysicalDevice pDev, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(pDev, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanRenderer::recordCommandBuffer(VkCommandBuffer buffer, int imageIdx)
{
    Logger &logger = ServiceLocator::getLogger();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS)
    {
        logger.error(MODULE_NAME, "Failed to begin record command buffer");
        throw std::runtime_error("Failed to begin record command buffer");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_vkRenderPass;
    renderPassInfo.framebuffer = m_vkSwapChainFramebuffers[imageIdx];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_vkSwapChainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_vkGraphicsPipeline);
        while(m_renderQueue.size() > 0)
        {
            VkRenderObject obj = m_renderQueue.front();
            if(obj.parent == NULL)
            {
                m_renderQueue.pop();
                continue;
            }

            VkBuffer vertexBuffers[] = {obj.vertexBuffer};
            VkDeviceSize offsets[] = {0};

            vkCmdPushConstants(buffer,
                               m_vkPipelineLayout,
                               VK_SHADER_STAGE_VERTEX_BIT,
                               0, sizeof(glm::mat4), &obj.parent->modelMatrix[0][0]);

            vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);
            vkCmdDraw(buffer, obj.parent->model->tris.size()*3, 1, 0, 0);

            m_renderQueue.pop();
        }
    vkCmdEndRenderPass(buffer);

    if(vkEndCommandBuffer(buffer) != VK_SUCCESS)
    {
        logger.error(MODULE_NAME, "Failed to end record command buffer");
        throw std::runtime_error("Failed to end record command buffer");
    }
}

void VulkanRenderer::createSurface()
{
    Logger &logger = ServiceLocator::getLogger();

    if(!m_nativeProps.isComplete())
    {
        logger.error(MODULE_NAME, "Native surface properties are not set");
        throw std::runtime_error("Native surface properties are not set");
    }
#ifdef __linux__
    VkXcbSurfaceCreateInfoKHR surfaceCreateInfo{};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.connection = m_nativeProps.connection.value();
    surfaceCreateInfo.window = m_nativeProps.window.value(); // glfwGetX11Window(mainWindow);

    if(vkCreateXcbSurfaceKHR(m_vkInstance, &surfaceCreateInfo, nullptr, &m_vkSurface) != VK_SUCCESS)
    {
        logger.error(MODULE_NAME, "Failed to create vulkan XCB surface");
        throw std::runtime_error("Failed to create vulkan XCB surface");
    }
#elif _WIN32
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = m_nativeProps.hwnd.value();
    createInfo.hinstance = m_nativeProps.hInstance.value();

    if(vkCreateWin32SurfaceKHR(m_vkInstance, &createInfo, nullptr, &m_vkSurface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vulkan win32 surface");
    }
#endif
}

void VulkanRenderer::setSurfaceProps(const NativeSurfaceProps &props)
{
    m_nativeProps = props;
}

void VulkanRenderer::addValidationLayer(const char *lay)
{
#ifndef NDEBUG
    m_validationLayers.push_back(lay);
#endif
}

void VulkanRenderer::draw()
{
    vkWaitForFences(m_vkDevice, 1, &m_vkInFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(m_vkDevice, 1, &m_vkInFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(m_vkDevice, m_vkSwapChain, UINT64_MAX, m_vkImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(m_vkCommandBuffer, /*VkCommandBufferResetFlagBits*/ 0);
    recordCommandBuffer(m_vkCommandBuffer, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_vkImageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_vkCommandBuffer;

    VkSemaphore signalSemaphores[] = {m_vkRenderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(m_vkGraphicsQueue, 1, &submitInfo, m_vkInFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_vkSwapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(m_vkPresentQueue, &presentInfo);
}

void VulkanRenderer::setRequiredFeatures(VkPhysicalDeviceFeatures feats)
{
    m_requiredFeatures = feats;
}

void VulkanRenderer::addInstanceExtension(const char* ext)
{
    m_instanceExtensions.push_back(ext);
}

void VulkanRenderer::addDeviceExtension(const char *ext)
{
    m_deviceExtensions.push_back(ext);
}
