#include <imgui.h>
#include <imgui_stdlib.h>

#include <backends/imgui_impl_glfw.h>
#include <spdlog/spdlog.h>
#include <boost/bind/bind.hpp>
#include <algorithm>

#include "client/uimanager.hpp"
#include "common/servicelocator.hpp"
#include "client/gamefrontendglfw.hpp"
#include "client/gamerendererdiligent.hpp"

#ifdef _WIN32
#include "Imgui/interface/ImGuiImplWin32.hpp"
#elif __linux__
#error "Not implemented"
#endif

#include "client/ui/uilabel.hpp"
#include "client/ui/uibutton.hpp"
#include "client/ui/uitextinput.hpp"

#include "Imgui/interface/ImGuiUtils.hpp"


UIManager* UIManager::create()
{
    return new UIManager();
}

UIManager::UIManager()
{

}

UIManager::~UIManager()
{

}

void UIManager::imgui_keybd_event(int key, int scancode, int action, int mods)
{
    // TODO: convert glfw key to ImGuiKey
}

void UIManager::imgui_mouse_button_event(int button, int action)
{
    ImGui::GetIO().AddMouseButtonEvent(button, action>0);
}

void UIManager::imgui_mouse_position_event(int dx, int dy)
{
    ImGui::GetIO().AddMousePosEvent(static_cast<float>(dx), static_cast<float>(dy));
}

void UIManager::init(Renderer *rend)
{
    m_renderer = (GameRendererDiligent*)rend;

    m_pImgui = Diligent::ImGuiImplWin32::Create(
        Diligent::ImGuiDiligentCreateInfo{m_renderer->m_pDevice, m_renderer->m_pSwapChain->GetDesc()},
        static_cast<HWND>(m_renderer->m_videoMode.osdata()));

    // bind events
    ServiceLocator::getEventManager().keyboardSubscribe(UIManager::imgui_keybd_event);
    ServiceLocator::getEventManager().mouseButtonSubscribe(UIManager::imgui_mouse_button_event);
    ServiceLocator::getEventManager().mousePositionSubscribe(UIManager::imgui_mouse_position_event);

    ImGui::StyleColorsClassic();
}

void UIManager::update(double dt)
{
    auto swapChainDesc = m_renderer->m_pSwapChain->GetDesc();
    
    m_pImgui->NewFrame(swapChainDesc.Width, swapChainDesc.Height, swapChainDesc.PreTransform);

    // TODO: for widget in widgets...
    ImGui::Begin("Main window");
    for(auto& el : m_elements)
    {
        switch (el->getType())
        {
        case UIElement::UIType::eLabel:
        {
            auto label = std::dynamic_pointer_cast<UILabel>(el);
            ImGui::Text(label->text().c_str());
            break;
        }
        /*
        case UIElement::UIType::eButton:
        {
            auto button = std::dynamic_pointer_cast<UIButton>(el);
            if (ImGui::Button(button->text().c_str()))
                button->onClick();
            break;
        }
        case UIElement::UIType::eTextInput:
        {
            auto textInput = std::dynamic_pointer_cast<UITextInput>(el);
            std::string val = textInput->getValue();
            if (ImGui::InputText(textInput->getLabel().c_str(), &val))
            {
                textInput->onChangeValue(val);
            }
            break;
        }
        */
        default:
            break;
        }
    }
    ImGui::End();
}

// this function is called from Renderer
void UIManager::draw()
{
    auto* pRTV = m_renderer->m_pSwapChain->GetCurrentBackBufferRTV();
    m_pImgui->Render(m_renderer->m_pImmediateContext);
}

void UIManager::addElement(std::shared_ptr<UIElement> el)
{
    m_elements.emplace_back(el);
}

void UIManager::removeElement(std::shared_ptr<UIElement> el)
{
    m_elements.erase(std::remove(m_elements.begin(), m_elements.end(), el), m_elements.end());
}
