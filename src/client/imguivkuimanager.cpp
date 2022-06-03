#include "client/imguivkuimanager.hpp"
#include "client/vulkanrenderer.hpp"
#include "common/servicelocator.hpp"

#ifdef CLIENT_GLFW
#include "imgui_impl_glfw.h"
#include "client/gameclientglfw.hpp"
#endif
#include "imgui_impl_vulkan.h"

static const char *MODULE_NAME = "ImguiVkUIManager";

UIManager *UIManager::create()
{
    ImGui::CreateContext();
    VulkanRenderer *rend = dynamic_cast<VulkanRenderer*>(&ServiceLocator::getRenderer());
#ifdef CLIENT_GLFW
    ImGui_ImplGlfw_InitForVulkan(GameClientGLFW::corePtr->getWindowPtr(), true);
#else
#error Not implemented
#endif
    return new ImguiVkUIManager(rend);
}

ImguiVkUIManager::ImguiVkUIManager(VulkanRenderer *rend)
    : m_renderer(rend),
      m_descPool(VK_NULL_HANDLE),
      m_onButtonPressed(nullptr)
{

}

ImguiVkUIManager::~ImguiVkUIManager()
{
    terminate();
}

void ImguiVkUIManager::init()
{
    Logger &logger = ServiceLocator::getLogger();

    ImGui::StyleColorsDark();

    // create descriptor pool
    {
        uint32_t maxDescriptorCount = 64;
        std::vector<vk::DescriptorPoolSize> pool_sizes =
        {
            vk::DescriptorPoolSize{ vk::DescriptorType::eSampler, maxDescriptorCount},
            vk::DescriptorPoolSize{ vk::DescriptorType::eCombinedImageSampler, maxDescriptorCount},
            vk::DescriptorPoolSize{ vk::DescriptorType::eSampledImage, maxDescriptorCount},
            vk::DescriptorPoolSize{ vk::DescriptorType::eStorageImage, maxDescriptorCount},
            vk::DescriptorPoolSize{ vk::DescriptorType::eUniformTexelBuffer, maxDescriptorCount},
            vk::DescriptorPoolSize{ vk::DescriptorType::eStorageTexelBuffer, maxDescriptorCount},
            vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBuffer, maxDescriptorCount},
            vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBuffer, maxDescriptorCount},
            vk::DescriptorPoolSize{ vk::DescriptorType::eUniformBufferDynamic, maxDescriptorCount},
            vk::DescriptorPoolSize{ vk::DescriptorType::eStorageBufferDynamic, maxDescriptorCount},
            vk::DescriptorPoolSize{ vk::DescriptorType::eInputAttachment, maxDescriptorCount}
        };
        vk::DescriptorPoolCreateInfo cInfo{vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
                                           maxDescriptorCount * static_cast<uint32_t>(pool_sizes.size()),
                                           pool_sizes};

        try
        {
            m_descPool = m_renderer->getDevice().createDescriptorPool(cInfo);
        }
        catch(const std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to create descriptor pool: " + std::string(e.what()));
            return;
        }
    }
    //

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = m_renderer->getInstance();
    initInfo.PhysicalDevice = m_renderer->getPhysicalDevice();
    initInfo.Device = m_renderer->getDevice();
    initInfo.QueueFamily = m_renderer->getQueueFamilyIndex();
    initInfo.Queue = m_renderer->getQueue();
    initInfo.PipelineCache = VK_NULL_HANDLE;
    initInfo.DescriptorPool = m_descPool;
    initInfo.Subpass = 0;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = m_renderer->getImageCount();
    switch(m_renderer->getSamplingValue())
    {
        case vk::SampleCountFlagBits::e1:
        initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        break;
        case vk::SampleCountFlagBits::e2:
        initInfo.MSAASamples = VK_SAMPLE_COUNT_2_BIT;
        break;
        case vk::SampleCountFlagBits::e4:
        initInfo.MSAASamples = VK_SAMPLE_COUNT_4_BIT;
        break;
        case vk::SampleCountFlagBits::e8:
        initInfo.MSAASamples = VK_SAMPLE_COUNT_8_BIT;
        break;
        case vk::SampleCountFlagBits::e16:
        initInfo.MSAASamples = VK_SAMPLE_COUNT_16_BIT;
        break;
        // really?
        case vk::SampleCountFlagBits::e32:
        initInfo.MSAASamples = VK_SAMPLE_COUNT_32_BIT;
        break;
        case vk::SampleCountFlagBits::e64:
        initInfo.MSAASamples = VK_SAMPLE_COUNT_64_BIT;
        break;
    }
    ImGui_ImplVulkan_Init(&initInfo, m_renderer->getRenderPass());

    { // load fonts
        vk::CommandBuffer cmdBuff = m_renderer->beginOneShotCmd();
        ImGui_ImplVulkan_CreateFontsTexture(cmdBuff);
        m_renderer->endOneShotCmd(cmdBuff);

        m_renderer->getDevice().waitIdle();
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}

void ImguiVkUIManager::terminate()
{
    m_renderer->getDevice().waitIdle();
    m_renderer->getDevice().destroyDescriptorPool(m_descPool);
    ImGui_ImplVulkan_Shutdown();
#ifdef CLIENT_GLFW
    ImGui_ImplGlfw_Shutdown();
#endif
    ImGui::DestroyContext();
}

void ImguiVkUIManager::update(double dt)
{
    (void)dt;
    // TODO
    ImGui_ImplVulkan_NewFrame();
#ifdef CLIENT_GLFW
    ImGui_ImplGlfw_NewFrame();
#endif
    ImGui::NewFrame();
    //
}

void ImguiVkUIManager::draw()
{
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_renderer->getCommandBuffer());
}

void ImguiVkUIManager::setOnButtonPressedCallback(OnButtonPressedCallback callb)
{
    m_onButtonPressed = callb;
}

void ImguiVkUIManager::OnButtonPressed(const ButtonData &data)
{
    if(m_onButtonPressed)
        m_onButtonPressed(data);
}
