#include "client/vkmaterial.hpp"
#include "common/servicelocator.hpp"

// precompiled shaders
#include "client/shaders/vk_color_shader.hpp"
#include "client/shaders/vk_image_shader.hpp"
#include "client/shaders/vk_pbr_specular_shader.hpp"
//

#include <fstream>
#include <memory>

static const char *MODULE_NAME = "VkMaterial";

Material *Material::create()
{
    VulkanRenderer *rend = dynamic_cast<VulkanRenderer*>(&ServiceLocator::getRenderer());
    return new VkMaterial(rend);
}

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

            if(m_lightDescPool)
                vkDevice.destroyDescriptorPool(m_lightDescPool);

            if(m_descPool)
                vkDevice.destroyDescriptorPool(m_descPool);

            for(auto &ubo : m_lightUBOs)
            {
                vkDevice.destroyBuffer(ubo.buffer);
                vkDevice.freeMemory(ubo.memory);
            }
            m_lightUBOs.clear();

            for(auto &ubo : m_UBOs)
            {
                vkDevice.destroyBuffer(ubo.buffer);
                vkDevice.freeMemory(ubo.memory);
            }
            m_UBOs.clear();

            // clear mat data
            if(std::holds_alternative<TextureObject>(m_visualData))
            {
                std::get<TextureObject>(m_visualData).free(vkDevice);
            }
            else if(std::holds_alternative<std::map<Material::TextureType, TextureObject>>(m_visualData))
            {
                auto &vec = std::get<std::map<Material::TextureType, TextureObject>>(m_visualData);
                for(auto &objPair : vec)
                    objPair.second.free(vkDevice);
                vec.clear();
            }

            if(m_lightDescSetLayout)
                vkDevice.destroyDescriptorSetLayout(m_lightDescSetLayout);

            vkDevice.destroyDescriptorSetLayout(m_descSetLayout);
        }
    }
}

void VkMaterial::setRenderer(VulkanRenderer *rend)
{
    if(rend)
        m_renderer = rend;
}

TextureObject VkMaterial::createTexture(const ImageData &imgData)
{
    Logger &logger = ServiceLocator::getLogger();

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
        return TextureObject{};
    }

    VkImageObject imgObj{};
    //
    try
    {
        uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(imgData.width, imgData.height)))) + 1;
        imgObj = m_renderer->createImage(imgData.width, imgData.height,
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
        return TextureObject{};
    }

    { // load texture into VRAM and generate mipmaps
        m_renderer->transitionImageLayout(imgObj,
                                          vk::ImageLayout::eUndefined,
                                          vk::ImageLayout::eTransferDstOptimal);
        m_renderer->copyBufferToImage(texStagingBufferObj.buffer, imgObj.image,
                                      imgObj.width, imgObj.height);

        m_renderer->getDevice().destroyBuffer(texStagingBufferObj.buffer);
        m_renderer->getDevice().freeMemory(texStagingBufferObj.memory);

        try
        {
            m_renderer->generateMipmaps(imgObj);
        }
        catch(const std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to generate mipmaps: " + std::string(e.what()));
            return TextureObject{};
        }
    }

    vk::ImageView imgView{};
    try
    {
        imgView = m_renderer->createImageView(imgObj);
    }
    catch(const std::exception &e)
    {
        logger.error(MODULE_NAME, "Failed to create vk::ImageView: " + std::string(e.what()));
        return TextureObject{};
    }
    return TextureObject(imgObj, imgView);
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
    switch(m_visualMode)
    {
        case Material::eColor:
        {
            vsh_data = vk_color_shader::vert_data();
            vsh_size = vk_color_shader::vert_size();
            fsh_data = vk_color_shader::frag_data();
            fsh_size = vk_color_shader::frag_size();
            break;
        }
        case Material::eDiffuse:
        {
            vsh_data = vk_image_shader::vert_data();
            vsh_size = vk_image_shader::vert_size();
            fsh_data = vk_image_shader::frag_data();
            fsh_size = vk_image_shader::frag_size();
            break;
        }
        case Material::MaterialMode::ePBR:
        {
            vsh_data = vk_pbr_specular_shader::vert_data();
            vsh_size = vk_pbr_specular_shader::vert_size();
            fsh_data = vk_pbr_specular_shader::frag_data();
            fsh_size = vk_pbr_specular_shader::frag_size();
            break;
        }
        default:
            break;
    }
    // load shaders
    {
        if(!vsh_data.has_value())
        {
            logger.error(MODULE_NAME, "Material properties haven't been initialized!");
            delete m_shader;
            m_shader = nullptr;
            return;
        }
        m_shader->load(vsh_data.value(), vsh_size.value(),
                       fsh_data.value(), fsh_size.value());
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
            vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer,
                                           1, vk::ShaderStageFlagBits::eVertex}
        };

        if(m_visualMode == Material::eDiffuse)
        {
            descSetLayoutBindings.emplace_back(1, vk::DescriptorType::eCombinedImageSampler,
                                               1, vk::ShaderStageFlagBits::eFragment);
        }
        else if(m_visualMode == Material::MaterialMode::ePBR)
        {
            // lighting data
            std::vector<vk::DescriptorSetLayoutBinding> lightDescSetLayoutBindings = {
                vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eUniformBuffer,
                                               1, vk::ShaderStageFlagBits::eFragment}
            };
            try
            {
                vk::DescriptorSetLayoutCreateInfo lightDescSetLayoutCInfo(vk::DescriptorSetLayoutCreateFlags(),
                                                                          lightDescSetLayoutBindings);
                m_lightDescSetLayout = vkDevice.createDescriptorSetLayout(lightDescSetLayoutCInfo);
            }
            catch(const std::exception &e)
            {
                logger.error(MODULE_NAME, "Failed to create vk::DescriptorSetLayout: " + std::string(e.what()));
                return;
            }

            // albedo
            descSetLayoutBindings.emplace_back(1, vk::DescriptorType::eCombinedImageSampler,
                                               1, vk::ShaderStageFlagBits::eFragment);
            // normal
            descSetLayoutBindings.emplace_back(2, vk::DescriptorType::eCombinedImageSampler,
                                               1, vk::ShaderStageFlagBits::eFragment);
            // roughness
            descSetLayoutBindings.emplace_back(3, vk::DescriptorType::eCombinedImageSampler,
                                               1, vk::ShaderStageFlagBits::eFragment);
            // metallic
            descSetLayoutBindings.emplace_back(4, vk::DescriptorType::eCombinedImageSampler,
                                               1, vk::ShaderStageFlagBits::eFragment);
            // ambient occlusion
            descSetLayoutBindings.emplace_back(5, vk::DescriptorType::eCombinedImageSampler,
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

        if(m_visualMode == Material::MaterialMode::eColor)
        {
            auto &color = std::get<glm::vec4>(m_visualData);
            if(color.w < 1.f)
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
        else if(m_visualMode == Material::MaterialMode::ePBR)
        {
            descSetLayouts.emplace_back(m_lightDescSetLayout);
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

        if(m_visualMode == Material::eDiffuse)
        {
            auto &texObj = std::get<TextureObject>(m_visualData);
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
                                               0.f, texObj.imgObj.mipLevels,
                                               vk::BorderColor::eIntOpaqueBlack,
                                               VK_FALSE);
            try
            {
                texObj.setSampler(vkDevice.createSampler(samplerCInfo));
            }
            catch(const std::exception &e)
            {
                logger.error(MODULE_NAME, "Failed to create vk::Sampler: " + std::string(e.what()));
                return;
            }
        }
        else if(m_visualMode == Material::MaterialMode::ePBR)
        {
            auto &textures = std::get<std::map<Material::TextureType, TextureObject>>(m_visualData);
            vk::PhysicalDeviceProperties props = m_renderer->getPhysicalDevice().getProperties();
            // diffuse
            auto samplerCreator = [&](const vk::Device &dev,
                                      Material::TextureType tt,
                                      const std::map<Material::TextureType, TextureObject> &textures,
                                      const vk::PhysicalDeviceProperties &props) -> vk::Sampler
            {
                vk::SamplerCreateInfo samplerCInfo(vk::SamplerCreateFlags(),
                                                   vk::Filter::eLinear,
                                                   vk::Filter::eLinear,
                                                   vk::SamplerMipmapMode::eLinear,
                                                   vk::SamplerAddressMode::eRepeat,
                                                   vk::SamplerAddressMode::eRepeat,
                                                   vk::SamplerAddressMode::eRepeat,
                                                   0.f, VK_TRUE, props.limits.maxSamplerAnisotropy,
                                                   VK_FALSE, vk::CompareOp::eAlways,
                                                   0.f, textures.at(tt).imgObj.mipLevels,
                                                   vk::BorderColor::eIntOpaqueBlack,
                                                   VK_FALSE);
                try
                {
                    return dev.createSampler(samplerCInfo);
                }
                catch(const std::exception &e)
                {
                    ServiceLocator::getLogger().error(MODULE_NAME, "Failed to create vk::Sampler: " + std::string(e.what()));
                    return vk::Sampler{};
                }
            };

            textures[Material::TextureType::eAlbedo].setSampler(samplerCreator(vkDevice, Material::TextureType::eAlbedo, textures, props));
            textures[Material::TextureType::eNormal].setSampler(samplerCreator(vkDevice, Material::TextureType::eNormal, textures, props));
            textures[Material::TextureType::eRoughness].setSampler(samplerCreator(vkDevice, Material::TextureType::eRoughness, textures, props));
            textures[Material::TextureType::eMetallic].setSampler(samplerCreator(vkDevice, Material::TextureType::eMetallic, textures, props));
            textures[Material::TextureType::eAmbientOcclusion].setSampler(samplerCreator(vkDevice, Material::TextureType::eAmbientOcclusion, textures, props));
        }

        // UniformBuffers
        for(size_t i=0; i < m_renderer->getImageCount(); i++)
        {
            try
            {
                {
                    VkBufferObject memObj = m_renderer->createBuffer(sizeof(SceneTransformData),
                                                                     vk::BufferUsageFlagBits::eUniformBuffer,
                                                                     vk::MemoryPropertyFlagBits::eHostVisible |
                                                                     vk::MemoryPropertyFlagBits::eHostCoherent);
                    m_UBOs.push_back(memObj);
                }

                if(m_visualMode == Material::MaterialMode::ePBR)
                {
                    VkBufferObject memObj = m_renderer->createBuffer(sizeof(LightingData),
                                                                     vk::BufferUsageFlagBits::eUniformBuffer,
                                                                     vk::MemoryPropertyFlagBits::eHostVisible |
                                                                     vk::MemoryPropertyFlagBits::eHostCoherent);
                    m_lightUBOs.push_back(memObj);
                }
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

        if(m_visualMode == Material::eDiffuse)
        {
            descPoolSizes.emplace_back(vk::DescriptorType::eCombinedImageSampler, imageCount);
        }
        else if(m_visualMode == Material::MaterialMode::ePBR)
        {
            descPoolSizes.emplace_back(vk::DescriptorType::eCombinedImageSampler, imageCount*5);
        }

        vk::DescriptorPoolCreateInfo descPoolCInfo{vk::DescriptorPoolCreateFlags{},
                                                   imageCount, descPoolSizes};
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
        try
        {
            {
                std::vector<vk::DescriptorSetLayout> setLayouts(imageCount, m_descSetLayout);
                vk::DescriptorSetAllocateInfo descSetAllocInfo(m_descPool, setLayouts);
                m_descSets = vkDevice.allocateDescriptorSets(descSetAllocInfo);
            }

            if(m_visualMode == Material::MaterialMode::ePBR)
            {
                std::vector<vk::DescriptorPoolSize> lightDescPoolSizes = {
                    vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, imageCount}
                };
                vk::DescriptorPoolCreateInfo lightDescPoolCInfo{vk::DescriptorPoolCreateFlags{},
                                                               imageCount, lightDescPoolSizes};
                m_lightDescPool = vkDevice.createDescriptorPool(lightDescPoolCInfo);

                std::vector<vk::DescriptorSetLayout> lightSetLayouts{imageCount, m_lightDescSetLayout};
                vk::DescriptorSetAllocateInfo lightDescSetAllocInfo{m_lightDescPool, lightSetLayouts};
                m_lightDescSets = vkDevice.allocateDescriptorSets(lightDescSetAllocInfo);
            }

            for(uint32_t i=0; i < imageCount; i++)
            {
                std::vector<vk::DescriptorImageInfo> imageInfos;
                std::vector<vk::DescriptorBufferInfo> bufferInfos =
                {
                    vk::DescriptorBufferInfo{m_UBOs[i].buffer,
                                             0,
                                             sizeof(SceneTransformData)}
                };
                std::vector<vk::DescriptorBufferInfo> lightBufferInfos;

                std::vector<vk::WriteDescriptorSet> writeSets =
                {
                    vk::WriteDescriptorSet{m_descSets[i],
                                           0, 0, vk::DescriptorType::eUniformBuffer,
                                           {}, bufferInfos, {}}
                };

                if(m_visualMode == Material::eDiffuse)
                {
                    auto &texture = std::get<TextureObject>(m_visualData);
                    imageInfos =
                    {
                        vk::DescriptorImageInfo{texture.imgSampler,
                                                texture.imgView,
                                                vk::ImageLayout::eShaderReadOnlyOptimal}
                    };
                    writeSets.push_back(vk::WriteDescriptorSet{
                                            m_descSets[i],
                                            1, 0, 1, vk::DescriptorType::eCombinedImageSampler,
                                            imageInfos.data(), VK_NULL_HANDLE, VK_NULL_HANDLE});
                }
                else if(m_visualMode == Material::MaterialMode::ePBR)
                {
                    lightBufferInfos = {
                        vk::DescriptorBufferInfo{m_lightUBOs[i].buffer,
                                                 0,
                                                 sizeof(LightingData)}
                    };
                    writeSets.push_back(vk::WriteDescriptorSet{
                                            m_lightDescSets[i],
                                            0, 0, vk::DescriptorType::eUniformBuffer,
                                            {}, lightBufferInfos, {}});

                    auto &textures = std::get<std::map<Material::TextureType, TextureObject>>(m_visualData);
                    imageInfos =
                    {
                        vk::DescriptorImageInfo{textures[Material::TextureType::eAlbedo].imgSampler,
                                                textures[Material::TextureType::eAlbedo].imgView,
                                                vk::ImageLayout::eShaderReadOnlyOptimal},
                        vk::DescriptorImageInfo{textures[Material::TextureType::eNormal].imgSampler,
                                                textures[Material::TextureType::eNormal].imgView,
                                                vk::ImageLayout::eShaderReadOnlyOptimal},
                        vk::DescriptorImageInfo{textures[Material::TextureType::eRoughness].imgSampler,
                                                textures[Material::TextureType::eRoughness].imgView,
                                                vk::ImageLayout::eShaderReadOnlyOptimal},
                        vk::DescriptorImageInfo{textures[Material::TextureType::eMetallic].imgSampler,
                                                textures[Material::TextureType::eMetallic].imgView,
                                                vk::ImageLayout::eShaderReadOnlyOptimal},
                        vk::DescriptorImageInfo{textures[Material::TextureType::eAmbientOcclusion].imgSampler,
                                                textures[Material::TextureType::eAmbientOcclusion].imgView,
                                                vk::ImageLayout::eShaderReadOnlyOptimal}
                    };
                    // albedo
                    writeSets.emplace_back(m_descSets[i],
                                           1, 0, 1, vk::DescriptorType::eCombinedImageSampler,
                                           imageInfos.data(), VK_NULL_HANDLE, VK_NULL_HANDLE);
                    // normal
                    writeSets.emplace_back(m_descSets[i],
                                           2, 0, 1, vk::DescriptorType::eCombinedImageSampler,
                                           imageInfos.data()+1, VK_NULL_HANDLE, VK_NULL_HANDLE);
                    // roughness
                    writeSets.emplace_back(m_descSets[i],
                                           3, 0, 1, vk::DescriptorType::eCombinedImageSampler,
                                           imageInfos.data()+2, VK_NULL_HANDLE, VK_NULL_HANDLE);
                    // metallic
                    writeSets.emplace_back(m_descSets[i],
                                           4, 0, 1, vk::DescriptorType::eCombinedImageSampler,
                                           imageInfos.data()+3, VK_NULL_HANDLE, VK_NULL_HANDLE);
                    // ambient occlusion
                    writeSets.emplace_back(m_descSets[i],
                                           5, 0, 1, vk::DescriptorType::eCombinedImageSampler,
                                           imageInfos.data()+4, VK_NULL_HANDLE, VK_NULL_HANDLE);
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

void VkMaterial::setPBR(const ImageData &albedo, const ImageData &normal,
                                 const ImageData &roughness, const ImageData &metallic,
                                 const ImageData &ambient)
{
    m_visualMode = Material::MaterialMode::ePBR;
    m_visualData = std::map<Material::TextureType, TextureObject>{
        {Material::TextureType::eAlbedo, createTexture(albedo)},
        {Material::TextureType::eNormal, createTexture(normal)},
        {Material::TextureType::eRoughness, createTexture(roughness)},
        {Material::TextureType::eMetallic, createTexture(metallic)},
        {Material::TextureType::eAmbientOcclusion, createTexture(ambient)}
    };
}

void VkMaterial::setImage(const ImageData &imgData, const std::string &name)
{
    (void)name;
    TextureObject texObj = createTexture(imgData);

    if(texObj.valid())
    {
        // set mode and image
        m_visualMode = Material::eDiffuse;
        m_visualData = texObj;
    }
    else
    {
        ServiceLocator::getLogger().error(MODULE_NAME, "Failed to create texture '" + name + "'");
    }
}

void VkMaterial::setColor(const glm::vec4 &color, const std::string &name)
{
    (void)name;
    // set mode and color
    m_visualMode = Material::eColor;
    m_visualData = color;
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
        {
            void *buffData = m_renderer->getDevice().mapMemory(m_UBOs[renderData.getImageIndex()].memory,
                                                               0,
                                                               m_UBOs[renderData.getImageIndex()].size,
                                                               vk::MemoryMapFlags());

            memcpy(buffData, reinterpret_cast<const void *>(&transformData), sizeof(SceneTransformData));
            m_renderer->getDevice().unmapMemory(m_UBOs[renderData.getImageIndex()].memory);
        }

        if(m_visualMode == Material::MaterialMode::ePBR)
        {
            void *buffData = m_renderer->getDevice().mapMemory(m_lightUBOs[renderData.getImageIndex()].memory,
                                                               0,
                                                               m_lightUBOs[renderData.getImageIndex()].size,
                                                               vk::MemoryMapFlags());

            const LightingData *lightData = m_renderer->getLightingData();
            memcpy(buffData, reinterpret_cast<const void *>(lightData), sizeof(LightingData));
            m_renderer->getDevice().unmapMemory(m_lightUBOs[renderData.getImageIndex()].memory);
        }
        m_lastImageIndex = renderData.getImageIndex();
    }

    vk::CommandBuffer &cmdBuff = renderData.getCmdBuffer();
    cmdBuff.bindPipeline(vk::PipelineBindPoint::eGraphics,
                         m_pipeline);
    cmdBuff.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                               m_pipelineLayout,
                               0, 1, &m_descSets[renderData.getImageIndex()],
                               0, VK_NULL_HANDLE);

    if(m_visualMode == Material::MaterialMode::ePBR)
    {
        cmdBuff.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                   m_pipelineLayout,
                                   1, 1, &m_lightDescSets[renderData.getImageIndex()],
                                   0, VK_NULL_HANDLE);
    }

    if(m_visualMode == Material::eColor)
    {
        struct ColorData {
            glm::mat4 model;
            glm::vec4 color;
        } colorData;
        colorData = {transformData.Model, std::get<glm::vec4>(m_visualData)};
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


