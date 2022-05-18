#include "vulkanrenderer.hpp"
#include "vkshader.hpp"
#include "servicelocator.hpp"
#include "vkmaterial.hpp"

#include <vector>
#include <optional>
#include <stdexcept>

static const char *MODULE_NAME = "VulkanRenderer";

VulkanRenderer::VulkanRenderer()
    : m_pDev(VK_NULL_HANDLE),
      m_queueInfo({}),
      m_currentSize(0, 0),
      m_vkImageFormat(vk::Format::eUndefined),
      m_currentFrame(0), m_defaultMaterial(nullptr),
      m_projMatrix(glm::mat4()), m_viewMatrix(glm::mat4())
{

}

VulkanRenderer::~VulkanRenderer()
{
    terminate();
}

void VulkanRenderer::terminate()
{
    if(!m_vkInstance)
        return;

    if(m_vkDevice)
    {
        m_vkDevice.waitIdle();

        for(auto &renderObject : m_createdObjects)
            delete renderObject;
        m_createdObjects.clear();

        for(auto &fence : m_vkFences)
            m_vkDevice.destroyFence(fence);
        m_vkFences.clear();

        for(auto &sigSem : m_vkSignalSemaphores)
            m_vkDevice.destroySemaphore(sigSem);
        m_vkSignalSemaphores.clear();

        for(auto &waitSem : m_vkWaitSemaphores)
            m_vkDevice.destroySemaphore(waitSem);
        m_vkWaitSemaphores.clear();

        if(m_vkDefaultRenderPass)
            m_vkDevice.destroyRenderPass(m_vkDefaultRenderPass);

        if(m_vkSwapchain)
        {
            for(auto &fbuff : m_vkSwapchainFramebuffers)
                m_vkDevice.destroyFramebuffer(fbuff);

            for(auto &imgView : m_vkSwapchainImageViews)
                m_vkDevice.destroyImageView(imgView);
            m_vkSwapchainImageViews.clear();

            m_vkSwapchainImages.clear();
            m_vkDevice.destroySwapchainKHR(m_vkSwapchain);
        }

        if(m_vkCmdPool)
            m_vkDevice.destroyCommandPool(m_vkCmdPool);

        m_vkQueues.clear();

        for(auto &mat : m_registeredMaterials)
            delete mat;

        m_vkDevice.destroy();
    }

    if(m_vkSurface)
        m_vkInstance.destroySurfaceKHR(m_vkSurface);

    m_vkInstance.destroy();
    ServiceLocator::getLogger().info(MODULE_NAME, "Successful cleanup");
}

void VulkanRenderer::init(const VideoMode &mode)
{
    Logger &logger = ServiceLocator::getLogger();
    (void)mode;
    { // instance
        vk::ApplicationInfo appInfo("", 0,
                                    "CleanEngine", VK_MAKE_VERSION(0, 0, 1),
                                    VK_API_VERSION_1_1);
        vk::InstanceCreateInfo cInfo(vk::InstanceCreateFlags(),
                                     &appInfo,
                                     m_validationLayers,
                                     m_instanceExtensions);
        try
        {
            m_vkInstance = vk::createInstance(cInfo);
        }
        catch(std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to create vk::Instance: " + std::string(e.what()));
            return terminate();
        }
        logger.info(MODULE_NAME, "Instance init completed");
    }

    { // physical device
        m_pDevices = m_vkInstance.enumeratePhysicalDevices();
        for(auto &dev : m_pDevices)
        {
            if(dev.getFeatures().geometryShader)
            {
                m_pDev = dev;
                break;
            }
        }

        if(!m_pDev)
        {
            logger.error(MODULE_NAME, "Failed to find suitable physical device");
            return terminate();
        }
        logger.info(MODULE_NAME, "Physical device init completed");
    }

    m_queueInfo.count = 0;

    { // queues
        auto queueFamilies = m_pDev.getQueueFamilyProperties();
        uint32_t qfId = 0;
        for(auto &qf : queueFamilies)
        {
            if((qf.queueFlags & vk::QueueFlagBits::eGraphics) &&
               (qf.queueFlags & vk::QueueFlagBits::eCompute))
            {
                if(qf.queueCount > m_queueInfo.count)
                {
                    m_queueInfo.familyIndex = qfId;
                    m_queueInfo.count = qf.queueCount;
                }
                break;
            }
            qfId++;
        }

        if(m_queueInfo.count == 0)
        {
            logger.error(MODULE_NAME, "Failed to find suitable queue family");
            return terminate();
        }
        logger.info(MODULE_NAME, "Queues init completed");
    }

    m_queueInfo.count = 1;

    { // logical device
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
        std::vector<float> queuePriorities(m_queueInfo.count, 1.f);
        queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(),
                                      m_queueInfo.familyIndex, queuePriorities);
        vk::PhysicalDeviceFeatures requiredFeatures = m_pDev.getFeatures();
        requiredFeatures.geometryShader = VK_TRUE;

        vk::DeviceCreateInfo cInfo(vk::DeviceCreateFlags(),
                                   queueCreateInfos,
                                   {},
                                   m_deviceExtensions,
                                   &requiredFeatures);
        try
        {
            m_vkDevice = m_pDev.createDevice(cInfo);
        }
        catch(std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to create vk::Device: " + std::string(e.what()));
            return terminate();
        }

        // get queues
        for(uint32_t i=0; i < m_queueInfo.count; i++)
        {
            vk::Queue queue;
            try
            {
                queue = m_vkDevice.getQueue(m_queueInfo.familyIndex, i);
            }
            catch(const std::exception &e)
            {
                logger.error(MODULE_NAME, "Failed to get vk::Queue: " + std::string(e.what()));
                return terminate();
            }
            m_vkQueues.push_back(queue);
        }
        logger.info(MODULE_NAME, "Logical device init completed");
    }

    { // command pool
        vk::CommandPoolCreateInfo cInfo(vk::CommandPoolCreateFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer),
                                  m_queueInfo.familyIndex);

        try
        {
            m_vkCmdPool = m_vkDevice.createCommandPool(cInfo);
        }
        catch(std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to create vk::CommandPool: " + std::string(e.what()));
            return terminate();
        }
        logger.info(MODULE_NAME, "Command pool init completed");
    }

    { // surface
        if(!m_nsp.isComplete())
        {
            logger.error(MODULE_NAME, "Failed to create SurfaceKHR");
            return terminate();
        }
#ifdef __linux__
        vk::XcbSurfaceCreateInfoKHR cInfo(vk::XcbSurfaceCreateFlagsKHR(),
                                          m_nsp.connection.value(),
                                          m_nsp.window.value());
        m_vkSurface = m_vkInstance.createXcbSurfaceKHR(cInfo);
#elif _WIN32
        vk::Win32SurfaceCreateInfoKHR cInfo(vk::Win32SurfaceCreateInfoKHR(),
                                            m_nsp.hwnd.value(),
                                            m_nsp.hInstance.value());
        m_vkSurface = m_vkInstance.createWin32SurfaceKHR(cInfo);
#endif
        logger.info(MODULE_NAME, "Surface init completed");
    }

    { // swapchain
        vk::SurfaceCapabilitiesKHR caps = m_pDev.getSurfaceCapabilitiesKHR(m_vkSurface);
        uint32_t imageCount = std::min(3u, caps.maxImageCount);
        vk::Extent2D imageExtent = caps.currentExtent;

        std::vector<vk::SurfaceFormatKHR> surfaceFormats = m_pDev.getSurfaceFormatsKHR(m_vkSurface);
        vk::ColorSpaceKHR imgColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
        for(auto &format : surfaceFormats)
        {
            if(format.colorSpace == imgColorSpace)
            {
                m_vkImageFormat = format.format;
                if(m_vkImageFormat == vk::Format::eB8G8R8A8Srgb)
                    break;
            }
        }

        vk::SwapchainCreateInfoKHR cInfo(vk::SwapchainCreateFlagsKHR(),
                                         m_vkSurface,
                                         imageCount,
                                         m_vkImageFormat,
                                         imgColorSpace,
                                         imageExtent,
                                         caps.maxImageArrayLayers,
                                         vk::ImageUsageFlags(vk::ImageUsageFlagBits::eColorAttachment),
                                         vk::SharingMode::eExclusive,
                                         1, &m_queueInfo.familyIndex,
                                         vk::SurfaceTransformFlagBitsKHR::eIdentity,
                                         vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                         vk::PresentModeKHR::eImmediate,
                                         VK_TRUE);
        try
        {
            m_vkSwapchain = m_vkDevice.createSwapchainKHR(cInfo);
        }
        catch(const std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to create vk::Swapchain: " + std::string(e.what()));
            return terminate();
        }

        try
        {
            m_vkSwapchainImages = m_vkDevice.getSwapchainImagesKHR(m_vkSwapchain);
        }
        catch(const std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to fetch vk::Image array: " + std::string(e.what()));
            return terminate();
        }

        m_currentSize = imageExtent;

        // create image views
        for(auto &img : m_vkSwapchainImages)
        {
            vk::ImageViewCreateInfo viewCInfo(vk::ImageViewCreateFlags(),
                                    img, vk::ImageViewType::e2D,
                                    m_vkImageFormat,
                                    vk::ComponentMapping{vk::ComponentSwizzle::eIdentity,
                                                         vk::ComponentSwizzle::eIdentity,
                                                         vk::ComponentSwizzle::eIdentity,
                                                         vk::ComponentSwizzle::eIdentity},
                                    vk::ImageSubresourceRange(vk::ImageAspectFlags(vk::ImageAspectFlagBits::eColor),
                                                              0, 1,
                                                              0, 1));
            try
            {
                m_vkSwapchainImageViews.push_back(m_vkDevice.createImageView(viewCInfo));
            }
            catch(const std::exception &e)
            {
                logger.error(MODULE_NAME, "Failed to create vk::ImageView: " + std::string(e.what()));
                return terminate();
            }
        }
        logger.info(MODULE_NAME, "Swapchain init completed");
    }

    { // command buffers
        vk::CommandBufferAllocateInfo aInfo(m_vkCmdPool,
                                            vk::CommandBufferLevel::ePrimary,
                                            m_vkSwapchainImages.size());

        try
        {
            m_vkCmdBuffers = m_vkDevice.allocateCommandBuffers(aInfo);
        }
        catch(const std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to allocate vk::CommandBuffer: " + std::string(e.what()));
            return terminate();
        }
        logger.info(MODULE_NAME, "Command buffers init completed");
    }

    { // render pass
        std::vector<vk::AttachmentDescription> attachments = {
            vk::AttachmentDescription{vk::AttachmentDescriptionFlags(),
                                      m_vkImageFormat,
                                      vk::SampleCountFlagBits::e1, // no MSAA
                                      vk::AttachmentLoadOp::eClear,
                                      vk::AttachmentStoreOp::eStore,
                                      vk::AttachmentLoadOp::eDontCare,
                                      vk::AttachmentStoreOp::eDontCare,
                                      vk::ImageLayout::eUndefined,
                                      vk::ImageLayout::ePresentSrcKHR}
        };

        vk::AttachmentReference mainAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

        std::vector<vk::SubpassDescription> subpassDescs = {
            vk::SubpassDescription{
                vk::SubpassDescriptionFlags(),
                vk::PipelineBindPoint::eGraphics,
                0, VK_NULL_HANDLE,
                1, &mainAttachmentRef,
                VK_NULL_HANDLE,
                VK_NULL_HANDLE,
                0, VK_NULL_HANDLE}
        };

        std::vector<vk::SubpassDependency> subpassDeps = {
            vk::SubpassDependency{VK_SUBPASS_EXTERNAL, 0,
                                  vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput),
                                  vk::PipelineStageFlags(vk::PipelineStageFlagBits::eColorAttachmentOutput),
                                  vk::AccessFlags(),
                                  vk::AccessFlags(vk::AccessFlagBits::eColorAttachmentWrite),
                                  vk::DependencyFlags()}
        };

        vk::RenderPassCreateInfo cInfo(vk::RenderPassCreateFlags(),
                                       attachments,
                                       subpassDescs,
                                       subpassDeps);

        try
        {
            m_vkDefaultRenderPass = m_vkDevice.createRenderPass(cInfo);
        }
        catch(const std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to create vk::RenderPass: " + std::string(e.what()));
            return terminate();
        }
        logger.info(MODULE_NAME, "RenderPass init completed");
    }

    { // create framebuffers
        for(auto &imgView : m_vkSwapchainImageViews)
        {
            vk::ImageView attachments[] = {
                imgView
            };
            vk::FramebufferCreateInfo fbuffCInfo(vk::FramebufferCreateFlags(),
                                                 m_vkDefaultRenderPass,
                                                 1, attachments,
                                                 m_currentSize.width, m_currentSize.height,
                                                 1);
            try
            {
                m_vkSwapchainFramebuffers.push_back(m_vkDevice.createFramebuffer(fbuffCInfo));
            }
            catch (const std::exception &e)
            {
                logger.error(MODULE_NAME, "Failed to create vk::Framebuffer: " + std::string(e.what()));
            }
        }
        logger.info(MODULE_NAME, "Framebuffers init completed");
    }

    { // sync objects
        vk::SemaphoreCreateInfo waitSemaphoreCInfo{vk::SemaphoreCreateFlags()};
        vk::SemaphoreCreateInfo signalSemaphoreCInfo{vk::SemaphoreCreateFlags()};
        vk::FenceCreateInfo fenceCInfo{vk::FenceCreateFlagBits::eSignaled};

        for(auto &_ : m_vkSwapchainImages)
        {
            try
            {
                m_vkWaitSemaphores.push_back(m_vkDevice.createSemaphore(waitSemaphoreCInfo));
                m_vkSignalSemaphores.push_back(m_vkDevice.createSemaphore(signalSemaphoreCInfo));
                m_vkFences.push_back(m_vkDevice.createFence(fenceCInfo));
            }
            catch (const std::exception &e)
            {
                logger.error(MODULE_NAME, "Failed to create sync object: " + std::string(e.what()));
            }
        }
        logger.info(MODULE_NAME, "Sync objects init completed");
    }

    {
        m_defaultMaterial = new VkMaterial(this);
        m_defaultMaterial->init();
    }

    logger.info(MODULE_NAME, "Renderer init completed");
}

void VulkanRenderer::queueRenderObject(const Model3D *obj, const glm::mat4 &modelMatrix)
{
    for(VkRenderObject *gObj : m_createdObjects)
    {
        if(gObj->getParent() == obj)
        {
            queueRenderObject(gObj, modelMatrix);
            return;
        }
    }
    VkRenderObject *gObj = createRenderObject(obj);
    if(gObj)
    {
        m_createdObjects.push_back(gObj);
        queueRenderObject(gObj, modelMatrix);
    }
}

void VulkanRenderer::queueRenderObject(VkRenderObject *obj, const glm::mat4 &modelMatrix)
{
    m_renderQueue.push(VkRenderRequest{obj, modelMatrix});
}

uint32_t VulkanRenderer::getMemoryType(uint32_t memoryBits, vk::MemoryPropertyFlags propFlags)
{
    vk::PhysicalDeviceMemoryProperties memProps = m_pDev.getMemoryProperties();
    for(uint32_t i=0; i < memProps.memoryTypeCount; i++)
    {
        if((memoryBits & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & propFlags) == propFlags)
            return i;
    }
    return 0;
}

VkBufferObject VulkanRenderer::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                                            vk::MemoryPropertyFlags memProps)
{
    vk::BufferCreateInfo cInfo(vk::BufferCreateFlags(),
                               size,
                               usage,
                               vk::SharingMode::eExclusive);
    vk::Buffer buffer = m_vkDevice.createBuffer(cInfo);

    vk::MemoryRequirements memReqs = m_vkDevice.getBufferMemoryRequirements(buffer);
    vk::MemoryAllocateInfo allocInfo(memReqs.size, getMemoryType(memReqs.memoryTypeBits, memProps));
    vk::DeviceMemory memory = m_vkDevice.allocateMemory(allocInfo);

    m_vkDevice.bindBufferMemory(buffer, memory, 0);
    return VkBufferObject{buffer, memory, size};
}

VkRenderObject *VulkanRenderer::createRenderObject(const Model3D *obj)
{
    auto *rObj = new VkRenderObject(m_vkDevice, obj);
    for(size_t i=0; i < obj->meshesCount; i++)
    {
        try
        {
            const Mesh3D &mesh = obj->pMeshes[i];
            uint32_t vertexDataSize = mesh.tris.size() * sizeof(Triangle3D);
            VkBufferObject memObj = createBuffer(vertexDataSize,
                                                 vk::BufferUsageFlagBits::eVertexBuffer,
                                                 vk::MemoryPropertyFlagBits::eHostVisible |
                                                 vk::MemoryPropertyFlagBits::eHostCoherent);
            void *dataPtr;
            m_vkDevice.mapMemory(memObj.memory, 0, memObj.size, vk::MemoryMapFlags(), &dataPtr);
            memcpy(dataPtr, mesh.tris.data(), vertexDataSize);
            m_vkDevice.unmapMemory(memObj.memory);
            rObj->addMesh({mesh.tris.size()*3, memObj});
        }
        catch(const std::exception &e)
        {
            ServiceLocator::getLogger().error(MODULE_NAME, "Failed to create VkRenderObject: " + std::string(e.what()));
            delete rObj;
            return nullptr;
        }
    }
    return rObj;
}

void VulkanRenderer::recordCommandBuffer(vk::CommandBuffer cmdBuff, uint32_t imgIndex)
{
    vk::CommandBufferBeginInfo beginInfo{};

    cmdBuff.begin(beginInfo);

    vk::ClearValue clearColor(std::array<float, 4>{0.f, 0.f, 0.f, 1.f});
    vk::RenderPassBeginInfo renderPassInfo(m_vkDefaultRenderPass,
                                           m_vkSwapchainFramebuffers[imgIndex],
                                           vk::Rect2D(vk::Offset2D(0, 0), m_currentSize),
                                           1, &clearColor);

    cmdBuff.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    // ------------------------------------- DRAWING -----------------------------------------------
    VkRenderData renderData {cmdBuff, imgIndex};
    while(!m_renderQueue.empty())
    {
        VkRenderRequest &req = m_renderQueue.front();
        const VkRenderObject *obj = req.getRenderObject();
        const Model3D *objParent = obj->getParent();

        for(auto &meshObj : obj->getMeshes())
        {
            TransformData td =
            {
                .Projection = m_projMatrix,
                .View = m_viewMatrix,
                .Model = req.getMatrix()
            };

            if(objParent->pMat)
            {
                auto *mat = dynamic_cast<VkMaterial*>(objParent->pMat);
                mat->use(td, renderData);
            }
            else
                m_defaultMaterial->use(td, renderData);

            vk::DeviceSize offsets[] = {0};
            cmdBuff.bindVertexBuffers(0, 1,
                                      &meshObj.memObj.buffer,
                                      offsets);
            cmdBuff.draw(meshObj.vertices, 1, 0, 0);
        }
        m_renderQueue.pop();
    }
    // =============================================================================================

    cmdBuff.endRenderPass();

    cmdBuff.end();
}

void VulkanRenderer::draw()
{
    Logger &logger = ServiceLocator::getLogger();

    vk::Result r;
    m_vkDevice.waitForFences(1, &m_vkFences[m_currentFrame],
                             VK_TRUE, UINT64_MAX);
    uint32_t imageIndex;
    r = m_vkDevice.acquireNextImageKHR(m_vkSwapchain,
                                       UINT64_MAX,
                                       m_vkWaitSemaphores[m_currentFrame],
                                       VK_NULL_HANDLE,
                                       &imageIndex);

    if(r != vk::Result::eSuccess)
    {
        logger.error(MODULE_NAME, "Failed to acquire image: " + vk::to_string(r));
        return;
    }

    m_vkDevice.resetFences(1, &m_vkFences[m_currentFrame]);
    vk::CommandBuffer &cmdBuff = m_vkCmdBuffers[m_currentFrame];
    cmdBuff.reset(vk::CommandBufferResetFlags());
    recordCommandBuffer(cmdBuff, imageIndex);

    vk::PipelineStageFlags waitStages[] = {
        vk::PipelineStageFlagBits::eColorAttachmentOutput
    };
    vk::SubmitInfo submitInfo(1, &m_vkWaitSemaphores[m_currentFrame],
                              waitStages,
                              1, &cmdBuff,
                              1, &m_vkSignalSemaphores[m_currentFrame]);

    r = m_vkQueues[0].submit(1, &submitInfo,
                             m_vkFences[m_currentFrame]);
    if(r != vk::Result::eSuccess)
    {
        logger.error(MODULE_NAME, "Failed to submit queue: " + vk::to_string(r));
        return terminate();
    }

    vk::PresentInfoKHR presentInfo(1, &m_vkSignalSemaphores[m_currentFrame],
                                   1, &m_vkSwapchain,
                                   &imageIndex);

    r = m_vkQueues[0].presentKHR(presentInfo);
    if(r != vk::Result::eSuccess)
    {
        logger.error(MODULE_NAME, "Failed to present image: " + vk::to_string(r));
        return terminate();
    }

    m_currentFrame = (m_currentFrame+1) % m_vkSwapchainImages.size();
}

glm::ivec2 VulkanRenderer::getSize() const
{
    return {m_currentSize.width, m_currentSize.height};
}

void VulkanRenderer::resize(const glm::ivec2 &size)
{
    std::stringstream infostr;
    infostr << "Resized: " << size.x << "x" << size.y;
    ServiceLocator::getLogger().info(MODULE_NAME, infostr.str());
}

void VulkanRenderer::setProjectionMatrix(const glm::mat4 &projmx)
{
    // update UBO?
    m_projMatrix = projmx;
}

void VulkanRenderer::setViewMatrix(const glm::mat4 &viewmx)
{
    // update UBO?
    m_viewMatrix = viewmx;
}

std::string VulkanRenderer::getType() const
{
    return VULKAN_RENDERER_TYPE;
}

void VulkanRenderer::addInstanceExtensions(const std::vector<const char *> &extensions)
{
    for(auto &ext : extensions)
        m_instanceExtensions.push_back(ext);
}

void VulkanRenderer::addDeviceExtensions(const std::vector<const char *> &extensions)
{
    for(auto &ext : extensions)
        m_deviceExtensions.push_back(ext);
}

void VulkanRenderer::addValidationLayers(const std::vector<const char *> &layers)
{
    for(auto &layer : layers)
        m_validationLayers.push_back(layer);
}

vk::Device &VulkanRenderer::getDevice()
{
    return m_vkDevice;
}

vk::PhysicalDevice &VulkanRenderer::getPhysicalDevice()
{
    return m_pDev;
}

vk::RenderPass &VulkanRenderer::getRenderPass()
{
    return m_vkDefaultRenderPass;
}

vk::Format VulkanRenderer::getImageFormat() const
{
    return m_vkImageFormat;
}

uint32_t VulkanRenderer::getImageCount() const
{
    return m_vkSwapchainImages.size();
}

uint32_t VulkanRenderer::getQueueFamilyIndex() const
{
    return m_queueInfo.familyIndex;
}

void VulkanRenderer::setNSP(const NativeSurfaceProps &nsp)
{
    m_nsp = nsp;
}

void VulkanRenderer::registerMaterial(VkMaterial *mat)
{
    if(mat)
        m_registeredMaterials.insert(mat);
}

VkImageObject VulkanRenderer::createImage(uint32_t width, uint32_t height, vk::Format format,
                                          vk::ImageTiling tiling,
                                          vk::ImageUsageFlags usage,
                                          vk::MemoryPropertyFlags memProps)
{
    vk::ImageCreateInfo imgCInfo(vk::ImageCreateFlags(),
                                 vk::ImageType::e2D,
                                 format,
                                 vk::Extent3D(width, height, 1),
                                 1, 1,
                                 vk::SampleCountFlagBits::e1,
                                 tiling,
                                 usage,
                                 vk::SharingMode::eExclusive,
                                 1, &m_queueInfo.familyIndex,
                                 vk::ImageLayout::eUndefined);
    vk::Image image = m_vkDevice.createImage(imgCInfo);

    vk::MemoryRequirements memReqs = m_vkDevice.getImageMemoryRequirements(image);
    vk::MemoryAllocateInfo allocInfo(memReqs.size, getMemoryType(memReqs.memoryTypeBits, memProps));
    vk::DeviceMemory memory = m_vkDevice.allocateMemory(allocInfo);

    m_vkDevice.bindImageMemory(image, memory, 0);
    return VkImageObject{image, memory, width, height, format};
}

vk::ImageView VulkanRenderer::createImageView(VkImageObject imageObject)
{
    vk::ImageViewCreateInfo imgViewCInfo(vk::ImageViewCreateFlags(),
                                         imageObject.image,
                                         vk::ImageViewType::e2D,
                                         imageObject.format,
                                         vk::ComponentMapping(),
                                         vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor,
                                                                   0, 1,
                                                                   0, 1));
    return m_vkDevice.createImageView(imgViewCInfo);
}

vk::CommandBuffer VulkanRenderer::beginOneShotCmd()
{
    vk::CommandBufferAllocateInfo allocInfo(m_vkCmdPool, vk::CommandBufferLevel::ePrimary, 1);

    vk::CommandBuffer cmdBuff;
    m_vkDevice.allocateCommandBuffers(&allocInfo, &cmdBuff);

    vk::CommandBufferBeginInfo beginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmdBuff.begin(beginInfo);
    return cmdBuff;
}

void VulkanRenderer::endOneShotCmd(vk::CommandBuffer cmdBuff)
{
    cmdBuff.end();

    vk::SubmitInfo submitInfo(0, VK_NULL_HANDLE, VK_NULL_HANDLE,
                              1, &cmdBuff,
                              0, VK_NULL_HANDLE);

    m_vkQueues[0].submit(1, &submitInfo,
                         VK_NULL_HANDLE);
    m_vkQueues[0].waitIdle();
    m_vkDevice.freeCommandBuffers(m_vkCmdPool, 1, &cmdBuff);
}

void VulkanRenderer::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
    vk::CommandBuffer cmdBuff = beginOneShotCmd();
    vk::BufferCopy copyRegion(0, 0, size);
    cmdBuff.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);
    endOneShotCmd(cmdBuff);
}

void VulkanRenderer::copyBufferToImage(vk::Buffer buffer, vk::Image image,
                                       uint32_t width, uint32_t height)
{
    vk::CommandBuffer cmdBuff = beginOneShotCmd();
    vk::BufferImageCopy region(0, 0, 0,
                               vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor,
                                                          0, 0, 1),
                               vk::Offset3D(0, 0, 0),
                               vk::Extent3D(width, height, 1));
    cmdBuff.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);
    endOneShotCmd(cmdBuff);
}
void VulkanRenderer::transitionImageLayout(VkImageObject imageObject,
                                           vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    vk::ImageMemoryBarrier barrier(vk::AccessFlags(), vk::AccessFlags(),
                                   oldLayout, newLayout,
                                   VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
                                   imageObject.image,
                                   vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor,
                                                             0, 1,
                                                             0, 1));
    vk::PipelineStageFlags srcStage{}, dstStage{};

    if (oldLayout == vk::ImageLayout::eUndefined &&
        newLayout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStage = vk::PipelineStageFlagBits::eTransfer;
    }
    else if(oldLayout == vk::ImageLayout::eTransferDstOptimal &&
            newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        srcStage = vk::PipelineStageFlagBits::eTransfer;
        dstStage = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else
    {
        std::stringstream errorstr;
        errorstr << "Unsupported image transition: " << vk::to_string(oldLayout) <<
                    " -> " << vk::to_string(newLayout);
        ServiceLocator::getLogger().error(MODULE_NAME, errorstr.str());
        return;
    }
    vk::CommandBuffer cmdBuff = beginOneShotCmd();
    cmdBuff.pipelineBarrier(srcStage, dstStage,
                            vk::DependencyFlags(),
                            0, VK_NULL_HANDLE,
                            0, VK_NULL_HANDLE,
                            1, &barrier);
    endOneShotCmd(cmdBuff);
}

