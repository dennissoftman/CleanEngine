#ifndef VKMATERIAL_HPP
#define VKMATERIAL_HPP

#include "material.hpp"
#include "vulkanrenderer.hpp"
#include "vkshader.hpp"

class VkMaterial : public Material
{
public:
    explicit VkMaterial(VulkanRenderer *rend=nullptr);
    ~VkMaterial() override;

    void init() override;

    void setImage(const std::string &path, const std::string &name) override;

    void use(TransformData &transformData) override;
    void use(TransformData &transformData, VkRenderData &renderData);

    void setDoubleSided(bool yes) override;
    [[nodiscard]] bool isDoubleSided() const override;

    void setRenderer(VulkanRenderer *rend);
private:
    VulkanRenderer *m_renderer; // associated renderer
    VkShader *m_shader;
    vk::DescriptorSetLayout m_descSetLayout;
    vk::PipelineLayout m_pipelineLayout;
    vk::Pipeline m_pipeline;

    //
    std::vector<VkBufferObject> m_UBOs;
    vk::DescriptorPool m_descPool;
    std::vector<vk::DescriptorSet> m_descSets;

    // TODO: textures
    std::optional<VkImageObject> m_image;
    std::optional<vk::ImageView> m_imageView;
    std::optional<vk::Sampler> m_textureSampler;
    //

    bool m_doubleSided{};
};

#endif // VKMATERIAL_HPP
