#include "vkmaterial.hpp"
#include "servicelocator.hpp"

#include <IL/il.h>
#include <IL/ilu.h>

static const char *MODULE_NAME = "VkMaterial";

VkMaterial::VkMaterial(VulkanRenderer *rend)
    : m_renderer(rend), m_shader(nullptr)
{

}

VkMaterial::~VkMaterial()
{
    if(m_renderer)
    {
        delete m_shader;
        if (m_pipeline)
        {
            vk::Device vkDevice = m_renderer->getDevice();
            vkDevice.destroyPipeline(m_pipeline);
            vkDevice.destroyPipelineLayout(m_pipelineLayout);

            if(m_descPool)
                vkDevice.destroyDescriptorPool(m_descPool);

            for(auto &ubo : m_UBOs)
            {
                vkDevice.destroyBuffer(ubo.buffer);
                vkDevice.freeMemory(ubo.memory);
            }
            m_UBOs.clear();

            if(m_textureSampler.has_value())
                vkDevice.destroySampler(m_textureSampler.value());

            if(m_imageView.has_value())
                vkDevice.destroyImageView(m_imageView.value());
            if(m_image.has_value())
            {
                vkDevice.destroyImage(m_image.value().image);
                vkDevice.freeMemory(m_image.value().memory);
            }

            vkDevice.destroyDescriptorSetLayout(m_descSetLayout);
        }
    }
}

void VkMaterial::setRenderer(VulkanRenderer *rend)
{
    if(rend)
        m_renderer = rend;
}

void VkMaterial::init()
{
    Logger &logger = ServiceLocator::getLogger();
    if (!m_renderer)
    {
        logger.error(MODULE_NAME, "No renderer assigned to material");
        return;
    }

    vk::Device &vkDevice = m_renderer->getDevice();
    m_shader = new VkShader(vkDevice);
    // TEMP
    if(m_image.has_value())
        m_shader->load("data/shaders/vk/main.vert.spv", "data/shaders/vk/image.frag.spv");
    else
        m_shader->load("data/shaders/vk/main.vert.spv", "data/shaders/vk/color.frag.spv");
    //

    { // create pipeline
        glm::vec2 extent = m_renderer->getSize(); // width x height

        std::vector<vk::PipelineShaderStageCreateInfo>
            shaderStagesCInfo = m_shader->getStagesCreateInfo();
        std::vector<vk::VertexInputBindingDescription> vertexBindings = {
            vk::VertexInputBindingDescription(0, sizeof(Vertex3D), vk::VertexInputRate::eVertex)
        };
        std::vector<vk::VertexInputAttributeDescription> vertexAttributes = {
            vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, 0), // vertCoord
            vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32Sfloat, sizeof(glm::vec3)),    // texCoord
            vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32B32Sfloat, sizeof(glm::vec3)+sizeof(glm::vec2))  // normCoord
        };

        vk::PipelineVertexInputStateCreateInfo
            vertexCInfo(vk::PipelineVertexInputStateCreateFlags(),
                        vertexBindings,
                        vertexAttributes);
        vk::PipelineInputAssemblyStateCreateInfo
            inputAssemblyCInfo(vk::PipelineInputAssemblyStateCreateFlags(),
                               vk::PrimitiveTopology::eTriangleList,
                               VK_FALSE);

        vk::PipelineTessellationStateCreateInfo
            tessellationStateCInfo(vk::PipelineTessellationStateCreateFlags(),
                                   0);

        vk::Viewport viewport(0.f, extent.y,
                              extent.x, -extent.y,
                              0.f, 1.f);
        vk::Rect2D scissors(vk::Offset2D(0, 0),
                            vk::Extent2D(extent.x, extent.y));
        vk::PipelineViewportStateCreateInfo
            viewportStateCInfo(vk::PipelineViewportStateCreateFlags(),
                               viewport, scissors);

        vk::PipelineRasterizationStateCreateInfo
            rasterizationStateCInfo(vk::PipelineRasterizationStateCreateFlags(),
                                    VK_FALSE, VK_FALSE,
                                    vk::PolygonMode::eFill,
                                    m_doubleSided ? vk::CullModeFlagBits::eNone : vk::CullModeFlagBits::eBack,
                                    vk::FrontFace::eCounterClockwise,
                                    VK_FALSE, 0.f, 0.f, 0.f, 0.f);

        vk::PipelineMultisampleStateCreateInfo
            multisampleStateCInfo(vk::PipelineMultisampleStateCreateFlags(),
                                  vk::SampleCountFlagBits::e1,
                                  VK_FALSE, 0.f,
                                  nullptr, VK_FALSE);

        vk::PipelineDepthStencilStateCreateInfo
            depthStencilStateCInfo(vk::PipelineDepthStencilStateCreateFlags(),
                                   VK_FALSE, VK_FALSE, vk::CompareOp::eNever, VK_FALSE,
                                   VK_FALSE);

        vk::PipelineColorBlendAttachmentState colorBlendAttachment(VK_FALSE);
        colorBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA);

        vk::PipelineColorBlendStateCreateInfo
            colorBlendStateCInfo(vk::PipelineColorBlendStateCreateFlags(),
                                 VK_FALSE,
                                 vk::LogicOp::eCopy,
                                 1, &colorBlendAttachment,
                                 {0.f, 0.f, 0.f, 0.f});

        std::vector<vk::DynamicState> dynamicStates = {
            // vk::DynamicState::eViewport
        };
        vk::PipelineDynamicStateCreateInfo dynamicStateCInfo(vk::PipelineDynamicStateCreateFlags(),
                                                             dynamicStates);

        // DescriptorSetLayouts
        std::vector<vk::DescriptorSetLayoutBinding> descSetLayoutBindings = {
            vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex)
        };

        if(m_image.has_value())
        {
            descSetLayoutBindings.emplace_back(1, vk::DescriptorType::eCombinedImageSampler,
                                               1, vk::ShaderStageFlagBits::eFragment);
        }

        vk::DescriptorSetLayoutCreateInfo descSetLayoutCInfo(vk::DescriptorSetLayoutCreateFlags(),
                                                             descSetLayoutBindings);
        try
        {
            m_descSetLayout = vkDevice.createDescriptorSetLayout(descSetLayoutCInfo);
        }
        catch(const std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to create vk::DescriptorSetLayout: " + std::string(e.what()));
            return;
        }
        //

        std::vector<vk::DescriptorSetLayout> descSetLayouts = {
            m_descSetLayout
        };
        std::vector<vk::PushConstantRange> pushConstantRanges;
        vk::PipelineLayoutCreateInfo pipelineLayoutCInfo(vk::PipelineLayoutCreateFlags(),
                                                         descSetLayouts,
                                                         pushConstantRanges);

        try
        {
            m_pipelineLayout = vkDevice.createPipelineLayout(pipelineLayoutCInfo);
        }
        catch (const std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to create vk::PipelineLayout: " + std::string(e.what()));
        }

        vk::GraphicsPipelineCreateInfo cInfo(vk::PipelineCreateFlags(),
                                             shaderStagesCInfo,
                                             &vertexCInfo,
                                             &inputAssemblyCInfo,
                                             &tessellationStateCInfo,
                                             &viewportStateCInfo,
                                             &rasterizationStateCInfo,
                                             &multisampleStateCInfo,
                                             &depthStencilStateCInfo,
                                             &colorBlendStateCInfo,
                                             &dynamicStateCInfo,
                                             m_pipelineLayout,
                                             m_renderer->getRenderPass());
        try
        {
            vk::ResultValue<vk::Pipeline> r = vkDevice.createGraphicsPipeline(VK_NULL_HANDLE,
                                                                              cInfo);
            if(r.result == vk::Result::eSuccess)
                m_pipeline = r.value;
            else
            {
                throw std::runtime_error(vk::to_string(r.result));
            }
        } catch(const std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to create vk::Pipeline: " + std::string(e.what()));
        }

        if(m_image.has_value()) // TODO: textures
        {
            vk::PhysicalDeviceProperties props = m_renderer->getPhysicalDevice().getProperties();

            vk::SamplerCreateInfo samplerCInfo(vk::SamplerCreateFlags(),
                                               vk::Filter::eLinear,
                                               vk::Filter::eLinear,
                                               vk::SamplerMipmapMode::eLinear,
                                               vk::SamplerAddressMode::eRepeat,
                                               vk::SamplerAddressMode::eRepeat,
                                               vk::SamplerAddressMode::eRepeat,
                                               0.f, VK_TRUE, props.limits.maxSamplerAnisotropy,
                                               VK_FALSE, vk::CompareOp::eAlways,
                                               0.f, 0.f,
                                               vk::BorderColor::eIntOpaqueBlack,
                                               VK_FALSE);
            try
            {
                m_textureSampler = vkDevice.createSampler(samplerCInfo);
            }
            catch(const std::exception &e)
            {
                logger.error(MODULE_NAME, "Failed to create vk::Sampler: " + std::string(e.what()));
                return;
            }
        }

        // UniformBuffers
        for(size_t i=0; i < m_renderer->getImageCount(); i++)
        {
            try
            {
                VkBufferObject memObj = m_renderer->createBuffer(sizeof(TransformData),
                                                                 vk::BufferUsageFlagBits::eUniformBuffer,
                                                                 vk::MemoryPropertyFlagBits::eHostVisible
                                                                     |
                                                                         vk::MemoryPropertyFlagBits::eHostCoherent);
                m_UBOs.push_back(memObj);
            }
            catch(const std::exception &e)
            {
                logger.error(MODULE_NAME, "Failed to create UniformBuffer: " + std::string(e.what()));
                return;
            }
        }
        // DescriptorPool
        uint32_t imageCount = m_renderer->getImageCount();
        std::vector<vk::DescriptorPoolSize> descPoolSizes = {
            vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, imageCount}
        };

        if(m_image.has_value())
        {
            descPoolSizes.emplace_back(vk::DescriptorType::eCombinedImageSampler, imageCount);
        }

        vk::DescriptorPoolCreateInfo descPoolCInfo(vk::DescriptorPoolCreateFlags(),
                                                   imageCount, descPoolSizes);
        try
        {
            m_descPool = vkDevice.createDescriptorPool(descPoolCInfo);
        }
        catch(const std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to create vk::DescriptorPool: " + std::string(e.what()));
            return;
        }
        // DescriptorSets
        std::vector<vk::DescriptorSetLayout> setLayouts(imageCount, m_descSetLayout);
        vk::DescriptorSetAllocateInfo descSetAllocInfo(m_descPool, setLayouts);
        try
        {
            m_descSets = vkDevice.allocateDescriptorSets(descSetAllocInfo);

            for(uint32_t i=0; i < imageCount; i++)
            {
                std::vector<vk::DescriptorImageInfo> imageInfos;
                std::vector<vk::DescriptorBufferInfo> bufferInfos =
                {
                    vk::DescriptorBufferInfo(m_UBOs[i].buffer,
                                             0,
                                             sizeof(TransformData))
                };

                std::vector<vk::WriteDescriptorSet> writeSets =
                {
                    vk::WriteDescriptorSet{m_descSets[i],
                                           0, 0, vk::DescriptorType::eUniformBuffer,
                                           {}, bufferInfos, {}}
                };

                if(m_image.has_value())
                {
                    imageInfos =
                    {
                        vk::DescriptorImageInfo{m_textureSampler.value(),
                                                m_imageView.value(),
                                                vk::ImageLayout::eShaderReadOnlyOptimal}
                    };
                    writeSets.push_back(vk::WriteDescriptorSet{m_descSets[i],
                                                               1, 0, vk::DescriptorType::eCombinedImageSampler,
                                                               imageInfos, {}, {}});
                }

                std::vector<vk::CopyDescriptorSet> copySets;
                vkDevice.updateDescriptorSets(writeSets, copySets);
            }
        }
        catch(const std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to allocate vk::DescriptorSet array: " + std::string(e.what()));
            return;
        }
    }
    m_renderer->registerMaterial(this);
}

void VkMaterial::setImage(const std::string &path, const std::string &name)
{
    Logger &logger = ServiceLocator::getLogger();
    // TODO: several images
    ILuint imgId = ilGenImage();
    ilBindImage(imgId);

    if(!ilLoadImage(path.c_str()))
    {
        logger.error(MODULE_NAME, "Image loading error");
        ilBindImage(0);
        ilDeleteImage(imgId);
        return;
    }

    iluFlipImage();

    ILint width  = ilGetInteger(IL_IMAGE_WIDTH),
          height = ilGetInteger(IL_IMAGE_HEIGHT);
    ILint bpp = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

    uint32_t imageDataSize = width * height * bpp;

    VkBufferObject texStagingBufferObj = m_renderer->createBuffer(imageDataSize,
                                                                  vk::BufferUsageFlagBits::eTransferSrc,
                                                                  vk::MemoryPropertyFlagBits::eHostVisible |
                                                                  vk::MemoryPropertyFlagBits::eHostCoherent);

    ILubyte *imageData = ilGetData();

    void *texData;
    m_renderer->getDevice().mapMemory(texStagingBufferObj.memory,
                                      0, texStagingBufferObj.size,
                                      vk::MemoryMapFlags(), &texData);
    memcpy(texData, imageData, imageDataSize);
    m_renderer->getDevice().unmapMemory(texStagingBufferObj.memory);

    ilBindImage(0);
    ilDeleteImage(imgId);

    //
    try
    {
        m_image = m_renderer->createImage(width, height, vk::Format::eR8G8B8A8Srgb,
                                          vk::ImageTiling::eOptimal,
                                          vk::ImageUsageFlagBits::eTransferDst |
                                              vk::ImageUsageFlagBits::eSampled,
                                          vk::MemoryPropertyFlagBits::eDeviceLocal);
    }
    catch(const std::exception &e)
    {
        logger.error(MODULE_NAME, "Failed to create vk::Image: " + std::string(e.what()));
        return;
    }

    { // load texture into VRAM
        m_renderer->transitionImageLayout(m_image.value(),
                                          vk::ImageLayout::eUndefined,
                                          vk::ImageLayout::eTransferDstOptimal);
        m_renderer
            ->copyBufferToImage(texStagingBufferObj.buffer, m_image.value().image, width, height);
        m_renderer->transitionImageLayout(m_image.value(),
                                          vk::ImageLayout::eTransferDstOptimal,
                                          vk::ImageLayout::eShaderReadOnlyOptimal);

        m_renderer->getDevice().destroyBuffer(texStagingBufferObj.buffer);
        m_renderer->getDevice().freeMemory(texStagingBufferObj.memory);
    }

    try
    {
        m_imageView = m_renderer->createImageView(m_image.value());
    }
    catch(const std::exception &e)
    {
        logger.error(MODULE_NAME, "Failed to create vk::ImageView: " + std::string(e.what()));
        return;
    }
}

void VkMaterial::use(TransformData &transformData)
{
    ServiceLocator::getLogger().error(MODULE_NAME,
                                      "Use 'use(TransformData&, VkRenderData&)' instead of 'use(TransformData&)'");
}

void VkMaterial::use(TransformData &transformData, VkRenderData &renderData)
{
    // TODO: ADD DIRTY FLAG
    {
        void *buffData;
        m_renderer->getDevice().mapMemory(m_UBOs[renderData.getImageIndex()].memory,
                                          0,
                                          m_UBOs[renderData.getImageIndex()].size,
                                          vk::MemoryMapFlags(),
                                          &buffData);
        memcpy(buffData, reinterpret_cast<void *>(&transformData), sizeof(TransformData));
        m_renderer->getDevice().unmapMemory(m_UBOs[renderData.getImageIndex()].memory);
    }
    //

    vk::CommandBuffer &cmdBuff = renderData.getCmdBuffer();
    cmdBuff.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                           m_pipeline);
    cmdBuff.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                               m_pipelineLayout,
                               0, 1, &m_descSets[renderData.getImageIndex()],
                               0, VK_NULL_HANDLE);
}

void VkMaterial::setDoubleSided(bool yes)
{
    m_doubleSided = yes;
}

bool VkMaterial::isDoubleSided() const
{
    return m_doubleSided;
}

