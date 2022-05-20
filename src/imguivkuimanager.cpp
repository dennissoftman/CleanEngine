#include "imguivkuimanager.hpp"
#include "servicelocator.hpp"
#include "vulkanrenderer.hpp"

#ifdef CORE_GLFW
#include "imgui_impl_glfw.h"
#endif
#include "imgui_impl_vulkan.h"

static const char *MODULE_NAME = "ImguiVkUIManager";

ImguiVkUIManager::ImguiVkUIManager()
    : m_descPool(VK_NULL_HANDLE),
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

    VulkanRenderer *renderer = static_cast<VulkanRenderer*>(&ServiceLocator::getRenderer());

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
                                           maxDescriptorCount * pool_sizes.size(),
                                           pool_sizes};

        try
        {
            m_descPool = renderer->getDevice().createDescriptorPool(cInfo);
        }
        catch(const std::exception &e)
        {
            logger.error(MODULE_NAME, "Failed to create descriptor pool: " + std::string(e.what()));
            return;
        }
    }
    //

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = renderer->getInstance();
    initInfo.PhysicalDevice = renderer->getPhysicalDevice();
    initInfo.Device = renderer->getDevice();
    initInfo.QueueFamily = renderer->getQueueFamilyIndex();
    initInfo.Queue = renderer->getQueue();
    initInfo.PipelineCache = VK_NULL_HANDLE;
    initInfo.DescriptorPool = m_descPool;
    initInfo.Subpass = 0;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = renderer->getImageCount();
    switch(renderer->getSamplingValue())
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
    ImGui_ImplVulkan_Init(&initInfo, renderer->getRenderPass());

    { // load fonts
        vk::CommandBuffer cmdBuff = renderer->beginOneShotCmd();
        ImGui_ImplVulkan_CreateFontsTexture(cmdBuff);
        renderer->endOneShotCmd(cmdBuff);

        renderer->getDevice().waitIdle();
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}

void ImguiVkUIManager::terminate()
{
    VulkanRenderer *renderer = static_cast<VulkanRenderer*>(&ServiceLocator::getRenderer());
    renderer->getDevice().waitIdle();
    renderer->getDevice().destroyDescriptorPool(m_descPool);
    ImGui_ImplVulkan_Shutdown();
    ImGui::DestroyContext();
}

void ImguiVkUIManager::update(double dt)
{
    (void)dt;
    // TODO
    ImGui_ImplVulkan_NewFrame();
#ifdef CORE_GLFW
    ImGui_ImplGlfw_NewFrame();
#endif
    ImGui::NewFrame();

    ImGui::Begin("Sample window!");
    ImGui::Text("This is your FPS: %.2lf", 1.0 / ((dt > 0) ? dt : 1.0));
    if(ImGui::Button("Generate Colliseum"))
        OnButtonPressed({0});
    if(ImGui::Button("Generate Dominoes"))
        OnButtonPressed({1});
    ImGui::End();
    //
}

void ImguiVkUIManager::draw(Renderer *rend)
{
    VulkanRenderer *renderer = static_cast<VulkanRenderer*>(rend);

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), renderer->getCommandBuffer());
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
