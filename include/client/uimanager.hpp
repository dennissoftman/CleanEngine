#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include <memory>

#include "client/renderer.hpp"
#include "client/ui/uielement.hpp"
#include "Imgui/interface/ImGuiImplDiligent.hpp"

class GameRendererDiligent;

class UIManager
{
    friend class Renderer;
public:
    UIManager();
    ~UIManager();

    void init(Renderer *rend);

    void update(double dt);
    void draw();

    void addElement(std::shared_ptr<UIElement> el);
    void removeElement(std::shared_ptr<UIElement> el);

    static UIManager *create();
private:
    static void imgui_keybd_event(int key, int scancode, int action, int mods);
    static void imgui_mouse_button_event(int button, int action);
    static void imgui_mouse_position_event(int dx, int dy);

    GameRendererDiligent* m_renderer;
    std::unique_ptr<Diligent::ImGuiImplDiligent> m_pImgui;
    std::vector<std::shared_ptr<UIElement>> m_elements;
};

#endif // UIMANAGER_HPP
