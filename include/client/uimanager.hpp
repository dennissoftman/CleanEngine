#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include "client/renderer.hpp"
#include "client/ui/uielement.hpp"

class UIManager
{
    friend class Renderer;
public:
    virtual ~UIManager() {}

    virtual void init(Renderer *rend) = 0;

    virtual void update(double dt) = 0;
    virtual void draw() =  0;

    virtual void addElement(std::shared_ptr<UIElement> el) = 0;
    virtual void removeElement(std::shared_ptr<UIElement> el) = 0;

    static UIManager *create();
};

#endif // UIMANAGER_HPP
