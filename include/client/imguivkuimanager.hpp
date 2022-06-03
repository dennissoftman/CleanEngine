#ifndef IMGUIVKUIMANAGER_HPP
#define IMGUIVKUIMANAGER_HPP

#include "client/uimanager.hpp"
#include "client/vulkanrenderer.hpp"

class ImguiVkUIManager : public UIManager
{
public:
    ImguiVkUIManager(VulkanRenderer *rend);
    ~ImguiVkUIManager() override;

    void init() override;

    void terminate();

    void update(double dt) override;
    void draw() override;

    void setOnButtonPressedCallback(OnButtonPressedCallback callb) override;
    void OnButtonPressed(const ButtonData &data);
private:
    VulkanRenderer *m_renderer;

    vk::DescriptorPool m_descPool;

    OnButtonPressedCallback m_onButtonPressed;
};

#endif // IMGUIVKUIMANAGER_HPP
