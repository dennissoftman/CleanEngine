#ifndef IMGUIVKUIMANAGER_HPP
#define IMGUIVKUIMANAGER_HPP

#include "client/uimanager.hpp"
#include "client/vulkanrenderer.hpp"
#include <functional>

class ImguiVkUIManager : public UIManager
{
public:
    ImguiVkUIManager();
    ~ImguiVkUIManager() override;

    void init(Renderer *rend) override;

    void terminate();

    void update(double dt) override;
    void draw() override;

    void addElement(std::shared_ptr<UIElement> el) override;
    void removeElement(std::shared_ptr<UIElement> el) override;
private:
    static void drawLabel(const char *text, const glm::vec2 &pos);
    static void drawButton(const char *text, const glm::vec2 &pos, const glm::vec2 &size, const std::function<void()> &callb);

    VulkanRenderer *m_renderer;

    vk::DescriptorPool m_descPool;

    std::vector<std::shared_ptr<UIElement>> m_elements;
};

#endif // IMGUIVKUIMANAGER_HPP
