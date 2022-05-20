#ifndef IMGUIVKUIMANAGER_HPP
#define IMGUIVKUIMANAGER_HPP

#include "uimanager.hpp"
#include <vulkan/vulkan.hpp>

class ImguiVkUIManager : public UIManager
{
public:
    ImguiVkUIManager();
    ~ImguiVkUIManager() override;

    void init() override;

    void terminate();

    void update(double dt) override;
    void draw(Renderer *rend) override;

    void setOnButtonPressedCallback(OnButtonPressedCallback callb) override;
    void OnButtonPressed(const ButtonData &data);
private:
    vk::DescriptorPool m_descPool;

    OnButtonPressedCallback m_onButtonPressed;
};

#endif // IMGUIVKUIMANAGER_HPP
