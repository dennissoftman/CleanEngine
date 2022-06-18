#ifndef VKMATERIAL_HPP
#define VKMATERIAL_HPP

#include "client/material.hpp"
#include "client/vulkanrenderer.hpp"
#include "client/vkshader.hpp"
#include "client/imageloader.hpp"

#include <map>
#include <variant>

struct SceneTransformData
{
    glm::mat4 projection;
    glm::mat4 view;
};

struct TextureObject
{
    TextureObject()
        : _allocated(false), _sampled(false)
    {

    }

    TextureObject(const VkImageObject &obj, const vk::ImageView &view)
        : imgObj(obj), imgView(view), _allocated(true), _sampled(false)
    { }

    void free(const vk::Device &dev)
    {
        if(_allocated)
        {
            if(_sampled)
                dev.destroySampler(imgSampler);

            dev.destroyImageView(imgView);
            dev.destroyImage(imgObj.image);
            dev.freeMemory(imgObj.memory);
            _allocated = false;
        }
    }

    void setSampler(const vk::Sampler &other)
    {
        imgSampler = other;
        _sampled = true;
    }

    bool valid()
    {
        return _allocated;
    }

    VkImageObject imgObj{};
    vk::ImageView imgView{};
    vk::Sampler imgSampler{};
    bool _allocated, _sampled;
};

class VkMaterial : public Material
{
public:
    explicit VkMaterial(VulkanRenderer *rend=nullptr);
    ~VkMaterial() override;

    void init() override;

    // material modes
    void setPBR(const ImageData &albedo, const ImageData &normal,
                const ImageData &roughness, const ImageData &metallic,
                const ImageData &ambient) override;
    void setPBR(const DataResource &pbrData) override;

    void setImage(const ImageData &imgData, const std::string &name) override;
    void setColor(const glm::vec4 &color, const std::string &name) override;
    //

    void use(TransformData &transformData) override;
    void use(TransformData &transformData, VkRenderData &renderData);

    void setDoubleSided(bool yes) override;
    [[nodiscard]] bool isDoubleSided() const override;

    void setRenderer(VulkanRenderer *rend);
private:
    TextureObject createTexture(const ImageData &imgData);
    bool m_wasInit;

    VulkanRenderer *m_renderer; // associated renderer
    VkShader *m_shader;
    vk::DescriptorSetLayout m_descSetLayout;
    vk::PipelineLayout m_pipelineLayout;
    vk::Pipeline m_pipeline;
    uint32_t m_lastImageIndex;

    //
    std::vector<VkBufferObject> m_UBOs;
    vk::DescriptorPool m_descPool;
    std::vector<vk::DescriptorSet> m_descSets;

    // PBR
    std::vector<VkBufferObject> m_lightUBOs;
    vk::DescriptorSetLayout m_lightDescSetLayout;
    vk::DescriptorPool m_lightDescPool;
    std::vector<vk::DescriptorSet> m_lightDescSets;

    // Material data
    Material::MaterialMode m_visualMode;
    std::variant<glm::vec4, TextureObject, std::map<Material::TextureType, TextureObject>> m_visualData;
    bool m_doubleSided{};
    //
};

#endif // VKMATERIAL_HPP
