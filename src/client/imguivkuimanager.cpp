#include "client/imguivkuimanager.hpp"
#include "client/vulkanrenderer.hpp"
#include "common/servicelocator.hpp"

#ifdef FRONTEND_GLFW
#include "imgui_impl_glfw.h"
#include "client/gamefrontendglfw.hpp"
#endif
#include "imgui_impl_vulkan.h"

#include "client/ui/uilabel.hpp"
#include "client/ui/uibutton.hpp"
#include "client/ui/uispinbox.hpp"

static const char *MODULE_NAME = "ImguiVkUIManager";

UIManager *UIManager::create()
{
    return new ImguiVkUIManager();
}

ImguiVkUIManager::ImguiVkUIManager()
    : m_renderer(nullptr), m_descPool(VK_NULL_HANDLE)
{

}

ImguiVkUIManager::~ImguiVkUIManager()
{
    terminate();
}

void ImguiVkUIManager::init(Renderer *rend)
{
    Logger &logger = ServiceLocator::getLogger();
    {
#ifdef FRONTEND_GLFW
        m_renderer = dynamic_cast<VulkanRenderer*>(rend);
        if(m_renderer == nullptr)
            throw std::runtime_error("renderer is null");

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForVulkan(((GameFrontendGLFW*)(GameFrontend::corePtr))->getWindowPtr(), true);
#else
#error UI without a frontend is a nonsence
#endif
    }

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
#ifdef FRONTEND_GLFW
    ImGui_ImplGlfw_Shutdown();
#endif
    ImGui::DestroyContext();
}

void ImguiVkUIManager::update(double dt)
{
    (void)dt;

    ImGui_ImplVulkan_NewFrame();

#ifdef FRONTEND_GLFW
    ImGui_ImplGlfw_NewFrame();
#endif
    ImGui::NewFrame();

    for(auto &el : m_elements)
    {
        switch(el->getType())
        {
            case UIElement::eLabel:
            {
                UILabel *lbl = dynamic_cast<UILabel*>(el.get());
                ImGui::Text(lbl->text().c_str());
                break;
            }
            case UIElement::eButton:
            {
                UIButton *btn = dynamic_cast<UIButton*>(el.get());
                if(ImGui::Button(btn->text().c_str()))
                {
                    btn->onClick();
                }
                break;
            }
            case UIElement::eSpinBox:
            {
                UISpinBox *spb = dynamic_cast<UISpinBox*>(el.get());
                float v = spb->getValue();
                if(ImGui::SliderFloat(spb->getLabel().c_str(), &v, spb->getMinimum(), spb->getMaximum(), "%.2f", 0.01f))
                {
                    spb->onChangeValue(v);
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

void ImguiVkUIManager::draw()
{
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_renderer->getCommandBuffer());
}

void ImguiVkUIManager::addElement(std::shared_ptr<UIElement> el)
{
    if(std::find(m_elements.begin(), m_elements.end(), el) == m_elements.end())
        m_elements.push_back(el);
}

void ImguiVkUIManager::removeElement(std::shared_ptr<UIElement> el)
{
    auto it = std::find(m_elements.begin(), m_elements.end(), el);
    if(it != m_elements.end())
        m_elements.erase(it);
}
