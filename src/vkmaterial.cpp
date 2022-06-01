#include "vkmaterial.hpp"
#include "servicelocator.hpp"

// precompiled shaders
#include "vk_color_shader.hpp"
#include "vk_image_shader.hpp"
//

#include <fstream>
#include <memory>

static const char *MODULE_NAME = "VkMaterial";

VkMaterial::VkMaterial(VulkanRenderer *rend)
    : m_wasInit(false), m_renderer(rend), m_shader(nullptr), m_lastImageIndex(~0u)
{

}

VkMaterial::~VkMaterial()
{
    if(m_renderer)
    {
        if (m_pipeline)
        {
            vk::Device vkDevice = m_renderer->getDevice();
            vkDevice.waitIdle();
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
    if(m_wasInit)
        return;

    Logger &logger = ServiceLocator::getLogger();
    if (!m_renderer)
    {
        logger.error(MODULE_NAME, "No renderer assigned to material");
        return;
    }

    vk::Device &vkDevice = m_renderer->getDevice();
    m_shader = new VkShader(vkDevice);
    // TEMP
    std::optional<const char *> vsh_data, fsh_data;
    std::optional<size_t> vsh_size, fsh_size;
    if(m_image.has_value())
    {
        vsh_data = vk_image_shader::vert_data();
        vsh_size = vk_image_shader::vert_size();
        fsh_data = vk_image_shader::frag_data();
        fsh_size = vk_image_shader::frag_size();
    }
    else if(m_color.has_value())
    {
        vsh_data = vk_color_shader::vert_data();
        vsh_size = vk_color_shader::vert_size();
        fsh_data = vk_color_shader::frag_data();
        fsh_size = vk_color_shader::frag_size();
    }
    else
    {
        logger.error(MODULE_NAME, "Specify either color or image");
        delete m_shader;
        m_shader = nullptr;
        return;
    }
    // load shaders
    {
        if(!vsh_data.has_value())
        {
            logger.error(MODULE_NAME, "Specify either color or image");
            delete m_shader;
            m_shader = nullptr;
            return;
        }
        m_shader->load(vsh_data.value(), vsh_size.value(),
                       fsh_data.value(), fsh_size.value());
        /*
        DataResource vdata = ServiceLocator::getResourceManager().getResource(vsh_path);
        DataResource fdata = ServiceLocator::getResourceManager().getResource(fsh_path);

        m_shader->load(static_pointer_cast<const char>(vdata.data).get(), vdata.size,
                       static_pointer_cast<const char>(fdata.data).get(), fdata.size);
        */
    }
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

        vk::SampleCountFlagBits samplingValue = m_renderer->getSamplingValue();
        vk::PipelineMultisampleStateCreateInfo
            multisampleStateCInfo(vk::PipelineMultisampleStateCreateFlags(),
                                  samplingValue,
                                  (samplingValue > vk::SampleCountFlagBits::e1) ? VK_TRUE : VK_FALSE, .2f,
                                  nullptr, VK_FALSE);

        vk::PipelineDepthStencilStateCreateInfo
            depthStencilStateCInfo(vk::PipelineDepthStencilStateCreateFlags(),
                                   VK_TRUE, VK_TRUE, vk::CompareOp::eLessOrEqual,
                                   VK_FALSE, VK_FALSE,
                                   {}, {},
                                   0.f, 1.f);

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
        std::vector<vk::PushConstantRange> pushConstantRanges = {
            vk::PushConstantRange(vk::ShaderStageFlagBits::eVertex,
                                  0, sizeof(glm::mat4))
        };

        if(m_color.has_value())
        {
            if(m_color.value().w < 1.f)
            {
                colorBlendAttachment.blendEnable = VK_TRUE;
                colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha;
                colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
                colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
                colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eSrcAlpha;
                colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha;
                colorBlendAttachment.alphaBlendOp = vk::BlendOp::eSubtract;
            }

            pushConstantRanges = {
                vk::PushConstantRange{vk::ShaderStageFlagBits::eVertex,
                                      0, sizeof(glm::mat4)+sizeof(glm::vec4)}
            };
        }

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
                                               0.f, m_image.value().mipLevels,
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
                VkBufferObject memObj = m_renderer->createBuffer(sizeof(SceneTransformData),
                                                                 vk::BufferUsageFlagBits::eUniformBuffer,
                                                                 vk::MemoryPropertyFlagBits::eHostVisible |
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
                                             sizeof(SceneTransformData))
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
    delete m_shader;
    m_shader = nullptr;
    m_wasInit = true;
    m_renderer->registerMaterial(this);
}

void VkMaterial::setImage(const ImageData &imgData, const std::string &name)
{
    Logger &logger = ServiceLocator::getLogger();

    if(m_color.has_value())
    {
        logger.error(MODULE_NAME, "Configurable materials are WIP, at the moment use either color or image");
        return;
    }

    (void)name;

    // TODO: several images
    VkBufferObject texStagingBufferObj = m_renderer->createBuffer(imgData.size,
                                                                  vk::BufferUsageFlagBits::eTransferSrc,
                                                                  vk::MemoryPropertyFlagBits::eHostVisible |
                                                                  vk::MemoryPropertyFlagBits::eHostCoherent);

    try
    {
        void *texData = m_renderer->getDevice().mapMemory(texStagingBufferObj.memory,
                                                          0, texStagingBufferObj.size,
                                                          vk::MemoryMapFlags());
        if(texData == nullptr)
            throw std::runtime_error("failed to map buffer memory");

        memcpy(texData, static_pointer_cast<void>(imgData.data).get(), imgData.size);
        m_renderer->getDevice().unmapMemory(texStagingBufferObj.memory);
    }
    catch(const std::exception &e)
    {
        logger.error(MODULE_NAME, "Failed to copy texture data: " + std::string(e.what()));
        return;
    }

    //
    try
    {
        uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(imgData.width, imgData.height)))) + 1;
        m_image = m_renderer->createImage(imgData.width, imgData.height,
                                          vk::Format::eR8G8B8A8Srgb, mipLevels,
                                          vk::ImageTiling::eOptimal,
                                          vk::ImageUsageFlagBits::eTransferSrc |
                                          vk::ImageUsageFlagBits::eTransferDst |
                                          vk::ImageUsageFlagBits::eSampled,
                                          vk::MemoryPropertyFlagBits::eDeviceLocal);
    }
    catch(const std::exception &e)
    {
        logger.error(MODULE_NAME, "Failed to create vk::Image: " + std::string(e.what()));
        return;
    }

    { // load texture into VRAM and generate mipmaps
        m_renderer->transitionImageLayout(m_image.value(),
                                          vk::ImageLayout::eUndefined,
                                          vk::ImageLayout::eTransferDstOptimal);
        m_renderer->copyBufferToImage(texStagingBufferObj.buffer, m_image.value().image,
                                      imgData.width, imgData.height);

        m_renderer->getDevice().destroyBuffer(texStagingBufferObj.buffer);
        m_renderer->getDevice().freeMemory(texStagingBufferObj.memory);

        try
        {
            m_renderer->generateMipmaps(m_image.value());
        }
        catch(const std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to generate mipmaps: " + std::string(e.what()));
            return;
        }
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

void VkMaterial::loadImage(const std::string &path, const std::string &name)
{
    ImageData imgData = ImageLoader::loadImage(path);
    setImage(imgData, name);
}

void VkMaterial::setColor(const glm::vec4 &color, const std::string &name)
{
    (void)name;

    Logger &logger = ServiceLocator::getLogger();
    if(m_image.has_value())
    {
        logger.error(MODULE_NAME, "Configurable materials are WIP, at the moment use either color or image");
        return;
    }
    m_color = color;
}

void VkMaterial::use(TransformData &transformData)
{
    (void)transformData;
    ServiceLocator::getLogger().error(MODULE_NAME,
                                      "Use 'use(TransformData&, VkRenderData&)' instead of 'use(TransformData&)'");
}

void VkMaterial::use(TransformData &transformData, VkRenderData &renderData)
{
    if(m_lastImageIndex != renderData.getImageIndex())
    {
        void *buffData = m_renderer->getDevice().mapMemory(m_UBOs[renderData.getImageIndex()].memory,
                                                           0,
                                                           m_UBOs[renderData.getImageIndex()].size,
                                                           vk::MemoryMapFlags());
        if(buffData == nullptr)
            throw std::runtime_error("Failed to map buffer memory");

        memcpy(buffData, reinterpret_cast<void *>(&transformData), sizeof(SceneTransformData));
        m_renderer->getDevice().unmapMemory(m_UBOs[renderData.getImageIndex()].memory);
        m_lastImageIndex = renderData.getImageIndex();
    }

    vk::CommandBuffer &cmdBuff = renderData.getCmdBuffer();
    cmdBuff.bindPipeline(vk::PipelineBindPoint::eGraphics,
                         m_pipeline);
    cmdBuff.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                               m_pipelineLayout,
                               0, 1, &m_descSets[renderData.getImageIndex()],
                               0, VK_NULL_HANDLE);

    if(m_color.has_value())
    {
        struct ColorData {
            glm::mat4 model;
            glm::vec4 color;
        } colorData;
        colorData = {transformData.Model, m_color.value()};
        cmdBuff.pushConstants(m_pipelineLayout,
                              vk::ShaderStageFlagBits::eVertex,
                              0, sizeof(ColorData), &colorData);
    }
    else
    {
        cmdBuff.pushConstants(m_pipelineLayout,
                              vk::ShaderStageFlagBits::eVertex,
                              0, sizeof(glm::mat4), &transformData.Model);
    }
}

void VkMaterial::setDoubleSided(bool yes)
{
    m_doubleSided = yes;
}

bool VkMaterial::isDoubleSided() const
{
    return m_doubleSided;
}


