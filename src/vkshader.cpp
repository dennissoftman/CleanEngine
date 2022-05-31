#include "vkshader.hpp"
#include "servicelocator.hpp"

#include <cstring>
#include <fstream>

static const char *MODULE_NAME = "VkShader";

VkShader::VkShader(vk::Device dev)
    : m_vkDevice(dev),
      m_vertModule(VK_NULL_HANDLE),
      m_fragModule(VK_NULL_HANDLE)
{

}

VkShader::~VkShader()
{
    if(m_vertModule)
        m_vkDevice.destroyShaderModule(m_vertModule);
    if(m_fragModule)
        m_vkDevice.destroyShaderModule(m_fragModule);
}

void VkShader::load(const char *vdata, size_t vsize, const char *fdata, size_t fsize)
{
    try
    {
        m_vertModule = createShader(vdata, vsize);
        m_fragModule = createShader(fdata, fsize);
    }
    catch(const std::exception &e)
    {
        ServiceLocator::getLogger().error(MODULE_NAME, "Failed to load shaders: " + std::string(e.what()));
    }
}

void VkShader::setDevice(const VkDevice &dev)
{
    m_vkDevice = dev;
}

std::vector<vk::PipelineShaderStageCreateInfo> VkShader::getStagesCreateInfo()
{
    return {
        vk::PipelineShaderStageCreateInfo(
                    vk::PipelineShaderStageCreateFlags(),
                    vk::ShaderStageFlagBits::eVertex,
                    m_vertModule, "main"),
        vk::PipelineShaderStageCreateInfo(
                    vk::PipelineShaderStageCreateFlags(),
                    vk::ShaderStageFlagBits::eFragment,
                    m_fragModule, "main")
    };
}

std::vector<char> VkShader::readShaderFile(const std::string &fname)
{
    std::ifstream fin(fname, std::ios::ate | std::ios::binary);
    if(!fin.is_open())
        throw std::runtime_error("Failed to open file '" + fname + "'");

    size_t vsDataSize = fin.tellg();
    std::vector<char> fData(vsDataSize);
    fin.seekg(0L);
    fin.read(fData.data(), vsDataSize);
    fin.close();

    return fData;
}

vk::ShaderModule VkShader::createShader(const char *code, size_t codeSize)
{
    vk::ShaderModuleCreateInfo cInfo(vk::ShaderModuleCreateFlags(),
                                     codeSize, reinterpret_cast<const uint32_t*>(code));
    return m_vkDevice.createShaderModule(cInfo);
}

void VkShader::load(const std::string &vs, const std::string &fs)
{
    std::vector<char> vsData = readShaderFile(vs),
                      fsData = readShaderFile(fs);

    try
    {
        m_vertModule = createShader(vsData.data(), vsData.size());
        m_fragModule = createShader(fsData.data(), fsData.size());
    }
    catch(const std::exception &e)
    {
        ServiceLocator::getLogger().error(MODULE_NAME, "Failed to create shader: " + std::string(e.what()));
    }
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
