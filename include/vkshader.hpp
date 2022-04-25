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

    void load(const char *vdata, int vsize, const char *fdata, int fsize) override;
    void load(const std::string &vs, const std::string &fs) override;

    void use() override;

    void setMat4(const std::string_view &id, const glm::mat4 &mat) override;
    void setInt(const std::string_view &id, int a) override;

    // vulkan
    void setDevice(const VkDevice &dev);

    VkShaderModule getVertexModule() const;
    VkShaderModule getFragmentModule() const;
private:
    static std::vector<char> readShaderFile(const std::string &fname);
    VkShaderModule createShader(const char *code, int codeSize);

    std::optional<VkDevice> m_vkDevice;
    VkShaderModule m_vertModule, m_fragModule;
};

#endif // VKSHADER_HPP
