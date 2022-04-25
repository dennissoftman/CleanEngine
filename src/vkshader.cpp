#include "vkshader.hpp"
#include <cstring>
#include <fstream>

VkShader::VkShader()
    : m_vkDevice(VK_NULL_HANDLE),
      m_vertModule(VK_NULL_HANDLE),
      m_fragModule(VK_NULL_HANDLE)
{

}

VkShader::~VkShader()
{
    vkDestroyShaderModule(m_vkDevice.value(), m_vertModule, nullptr);
    vkDestroyShaderModule(m_vkDevice.value(), m_fragModule, nullptr);
}

void VkShader::load(const char *vdata, int vsize, const char *fdata, int fsize)
{
    if(!m_vkDevice.has_value())
        throw std::runtime_error("Cannot create shader without vulkan device");

    m_vertModule = createShader(vdata, vsize);
    m_fragModule = createShader(fdata, fsize);
}

void VkShader::setDevice(const VkDevice &dev)
{
    m_vkDevice = dev;
}

VkShaderModule VkShader::getVertexModule() const
{
    return m_vertModule;
}

VkShaderModule VkShader::getFragmentModule() const
{
    return m_fragModule;
}

std::vector<char> VkShader::readShaderFile(const std::string &fname)
{
    std::ifstream fin(fname, std::ios::ate | std::ios::binary);
    if(!fin.is_open())
        throw std::runtime_error("Failed to load vertex shader file '" + fname + "'");

    size_t vsDataSize = fin.tellg();
    std::vector<char> fData(vsDataSize);
    fin.seekg(0L);
    fin.read(fData.data(), vsDataSize);
    fin.close();

    return fData;
}

VkShaderModule VkShader::createShader(const char *code, int codeSize)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = codeSize;
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code);

    VkShaderModule module;
    if(vkCreateShaderModule(m_vkDevice.value(), &createInfo, nullptr, &module) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader module");
    return module;
}

void VkShader::load(const std::string &vs, const std::string &fs)
{
    if(!m_vkDevice.has_value())
        throw std::runtime_error("Cannot create shader without vulkan device");

    std::vector<char> vsData = readShaderFile(vs),
                      fsData = readShaderFile(fs);

    m_vertModule = createShader(vsData.data(), vsData.size());
    m_fragModule = createShader(fsData.data(), fsData.size());
}

void VkShader::use()
{

}

void VkShader::setMat4(const std::string_view &id, const glm::mat4 &mat)
{
    // push constants or uniform buffer objects
}

void VkShader::setInt(const std::string_view &id, int a)
{
    // push constants?
}
