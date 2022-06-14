#ifndef DUMMYUIMANAGER_HPP
#define DUMMYUIMANAGER_HPP

#include "client/uimanager.hpp"

class DummyUIManager : public UIManager
{
public:
    DummyUIManager();

    void init(Renderer *rend) override;
    void update(double dt) override;
    void draw() override;

    void addElement(std::shared_ptr<UIElement> el) override;
    void removeElement(std::shared_ptr<UIElement> el) override;
};

#endif // DUMMYUIMANAGER_HPP
