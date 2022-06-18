#ifndef VULKANRENDERER_HPP
#define VULKANRENDERER_HPP

#include "client/renderer.hpp"

#ifdef __linux__
#define VK_USE_PLATFORM_XCB_KHR
#include <X11/Xlib-xcb.h>
#elif _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#include <Windows.h>
#endif

#include <vulkan/vulkan.hpp>
#include <vector>
#include <queue>
#include <set>
#include <optional>

#define VULKAN_RENDERER_TYPE "vk"

struct NativeSurfaceProps
{
#ifdef __linux__
    std::optional<xcb_connection_t*> connection;
    std::optional<xcb_window_t> window;

    [[nodiscard]] bool isComplete() const
    {
        return connection.has_value() && window.has_value();
    }
#elif _WIN32
    std::optional<HWND> hwnd;
    std::optional<HINSTANCE> hInstance;

    [[nodiscard]] bool isComplete() const
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

struct VkBufferObject
{
    vk::Buffer buffer;
    vk::DeviceMemory memory;
    vk::DeviceSize size;
};

struct VkImageObject
{
    vk::Image image;
    vk::DeviceMemory memory;
    uint32_t width, height;
    vk::Format format;
    uint32_t mipLevels = 1;
};

struct VkMeshData
{
    size_t vertices;
    VkBufferObject memObj;
};

class VkMaterial;

class VkRenderObject
{
public:
    explicit VkRenderObject(vk::Device dev, const Model3D *parent)
        : m_vkDevice(dev), m_parent(parent)
    {}

    ~VkRenderObject()
    {
        for(auto &mesh : m_meshes)
        {
            m_vkDevice.destroyBuffer(mesh.first.memObj.buffer);
            m_vkDevice.freeMemory(mesh.first.memObj.memory);
        }
        m_meshes.clear();
    }

    const std::vector<std::pair<VkMeshData, VkMaterial*>> &getMeshes() const
    {
        return m_meshes;
    }

    [[nodiscard]] const Model3D *getParent() const
    {
        return m_parent;
    }

    void addMesh(const VkMeshData &mesh, VkMaterial *mat=nullptr)
    {
        m_meshes.push_back(std::pair<VkMeshData, VkMaterial*>(mesh, mat));
    }
private:
    vk::Device m_vkDevice;
    const Model3D *m_parent;
    std::vector<std::pair<VkMeshData, VkMaterial*>> m_meshes;
};

class VkRenderRequest
{
public:
    explicit VkRenderRequest(VkRenderObject *obj, const glm::mat4 &modelMatrix)
        : m_renderObject(obj), m_modelMatrix(modelMatrix)
    { }

    [[nodiscard]] const VkRenderObject *getRenderObject() const
    {
        return m_renderObject;
    }

    [[nodiscard]] const glm::mat4 &getMatrix() const
    {
        return m_modelMatrix;
    }
private:
    VkRenderObject *m_renderObject;
    glm::mat4 m_modelMatrix;
};

struct VkShaderTransformData
{
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;
};

class VkRenderData : public RenderData
{
public:
    explicit VkRenderData(vk::CommandBuffer cmdBuff, uint32_t imageIndex=0)
        : m_cmdBuff(cmdBuff), m_imageIndex(imageIndex)
    {}

    [[nodiscard]] std::string getType() const override
    {
        return VULKAN_RENDERER_TYPE;
    }

    [[nodiscard]] vk::CommandBuffer &getCmdBuffer()
    {
        return m_cmdBuff;
    }

    [[nodiscard]] uint32_t getImageIndex()
    {
        return m_imageIndex;
    }
private:
    vk::CommandBuffer m_cmdBuff;
    uint32_t m_imageIndex;
};

struct QueueFamilyInfo
{
    uint32_t familyIndex;
    uint32_t count; // queue count
};

class Camera3D;

class VulkanRenderer : public Renderer
{
public:
    VulkanRenderer();
    ~VulkanRenderer() override;

    void init(const VideoMode &mode) override;
    void terminate();

    void queueRenderObject(const Model3D *obj, const glm::mat4 &modelMatrix) override;
    void queueRenderObject(VkRenderObject *obj, const glm::mat4 &modelMatrix);
    void draw() override;

    [[nodiscard]] glm::ivec2 getSize() const override;
    void resize(const glm::ivec2 &size) override;

    void updateCameraData(Camera3D &cam) override;
    void updateLightCount(uint32_t count) override;
    void updateLightPosition(const glm::vec4 &pos, uint32_t id=0) override;
    void updateLightColor(const glm::vec4 &color, uint32_t id=0) override;

    [[nodiscard]] std::string getType() const override;

    void setNSP(const NativeSurfaceProps &nsp);

    // vulkan-related
    void addInstanceExtensions(const std::vector<const char*> &extensions);
    void addDeviceExtensions(const std::vector<const char*> &extensions);
    void addValidationLayers(const std::vector<const char*> &layers);

    [[nodiscard]] vk::Instance &getInstance();
    [[nodiscard]] vk::PhysicalDevice &getPhysicalDevice();
    [[nodiscard]] vk::Device &getDevice();
    [[nodiscard]] vk::Queue &getQueue();
    [[nodiscard]] vk::RenderPass &getRenderPass();
    [[nodiscard]] vk::Format getImageFormat() const;
    [[nodiscard]] vk::SampleCountFlagBits getSamplingValue() const;
    [[nodiscard]] uint32_t getImageCount() const;
    [[nodiscard]] uint32_t getQueueFamilyIndex() const;
    [[nodiscard]] vk::CommandBuffer getCommandBuffer() const; // current drawing command buffer

    [[nodiscard]] VkBufferObject createBuffer(vk::DeviceSize size,
                                              vk::BufferUsageFlags usage,
                                              vk::MemoryPropertyFlags memProps);
    [[nodiscard]] VkImageObject createImage(uint32_t width, uint32_t height,
                                            vk::Format format,
                                            uint32_t mipLevels,
                                            vk::ImageTiling tiling,
                                            vk::ImageUsageFlags usage,
                                            vk::MemoryPropertyFlags memProps,
                                            vk::SampleCountFlagBits samples=vk::SampleCountFlagBits::e1);
    void generateMipmaps(const VkImageObject &imageObject);

    const LightingData *getLightingData() const;

    [[nodiscard]] vk::ImageView createImageView(const VkImageObject &imageObject,
                                                vk::ImageAspectFlags aspect=vk::ImageAspectFlagBits::eColor);
    void registerMaterial(VkMaterial *mat);

    void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
    void transitionImageLayout(VkImageObject imageObject,
                               vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
    void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

    vk::CommandBuffer beginOneShotCmd();
    void endOneShotCmd(vk::CommandBuffer cmdBuff);
private:
    void recordCommandBuffer(vk::CommandBuffer cmdBuff, uint32_t imgIndex);
    uint32_t getMemoryType(uint32_t memTypeBits, vk::MemoryPropertyFlags propFlags);
    [[nodiscard]] VkRenderObject *createRenderObject(const Model3D *obj);

    std::vector<const char*> m_instanceExtensions, m_deviceExtensions;
    std::vector<const char*> m_validationLayers;

    vk::Instance m_vkInstance;
    std::vector<vk::PhysicalDevice> m_pDevices;
    vk::PhysicalDevice m_pDev;
    vk::Device m_vkDevice;
    QueueFamilyInfo m_queueInfo;
    std::vector<vk::Queue> m_vkQueues;
    NativeSurfaceProps m_nsp;
    vk::Extent2D m_currentSize; // surface size
    vk::SurfaceKHR m_vkSurface;
    vk::SwapchainKHR m_vkSwapchain;
    vk::Format m_vkImageFormat;
    std::vector<vk::Image> m_vkSwapchainImages;
    std::vector<vk::ImageView> m_vkSwapchainImageViews;
    vk::Format m_vkDepthFormat;
    VkImageObject m_vkDepthImageObject;
    vk::ImageView m_vkDepthImageView;
    VkImageObject m_vkMultisampleObject;
    vk::ImageView m_vkMultisampleView;
    vk::SampleCountFlagBits m_samplingValue;
    std::vector<vk::Framebuffer> m_vkSwapchainFramebuffers;
    uint32_t m_currentFrame;
    vk::CommandPool m_vkCmdPool;
    std::vector<vk::CommandBuffer> m_vkCmdBuffers;
    vk::RenderPass m_vkDefaultRenderPass;
    VkMaterial *m_defaultMaterial;
    // sync objects
    std::vector<vk::Fence> m_vkFences;
    std::vector<vk::Semaphore> m_vkWaitSemaphores, m_vkSignalSemaphores;

    // render objects
    std::set<VkMaterial*> m_registeredMaterials;
    std::vector<VkRenderObject*> m_createdObjects;
    std::queue<VkRenderRequest> m_renderQueue;
    glm::mat4 m_projMatrix, m_viewMatrix;

    LightingData m_lightingData;
};

#endif // VULKANRENDERER_HPP
