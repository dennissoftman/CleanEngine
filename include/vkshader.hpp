#ifndef VKSHADER_HPP
#define VKSHADER_HPP

#include "shader.hpp"
#include <vulkan/vulkan.hpp>
#include <vector>
#include <optional>

class VkShader : public Shader
{
public:
    explicit VkShader(vk::Device dev);
    ~VkShader();

    void load(const char *vdata, size_t vsize, const char *fdata, size_t fsize) override;
    void load(const std::string &vs, const std::string &fs) override;

    void use() override;

    void setMat4(const std::string_view &id, const glm::mat4 &mat) override;
    void setInt(const std::string_view &id, int a) override;

    // vulkan
    void setDevice(const VkDevice &dev);

    std::vector<vk::PipelineShaderStageCreateInfo> getStagesCreateInfo();
private:
    static std::vector<char> readShaderFile(const std::string &fname);
    vk::ShaderModule createShader(const char *code, size_t codeSize);

    vk::Device m_vkDevice;
    vk::ShaderModule m_vertModule, m_fragModule;
};

#endif // VKSHADER_HPP
