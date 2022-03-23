#ifndef VKSHADER_HPP
#define VKSHADER_HPP

#include "shader.hpp"
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

class VkShader : public Shader
{
public:
    VkShader();
    ~VkShader();

    void load(const std::string &vs, const std::string &fs) override;

    void use() override;

    void setDevice(const VkDevice &dev);

    VkShaderModule getVertexModule() const;
    VkShaderModule getFragmentModule() const;
private:
    static std::vector<char> readShaderFile(const std::string &fname);
    VkShaderModule createShader(const std::vector<char> &code);

    std::optional<VkDevice> m_vkDevice;
    VkShaderModule m_vertModule, m_fragModule;
};

#endif // VKSHADER_HPP
