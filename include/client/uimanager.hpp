#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include "client/renderer.hpp"
#include "client/ui/uielement.hpp"

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
};

#endif // UIMANAGER_HPP
