#ifndef DUMMYUIMANAGER_HPP
#define DUMMYUIMANAGER_HPP

#include "uimanager.hpp"

class DummyUIManager : public UIManager
{
public:
    DummyUIManager();

    void init() override;
    void update(double dt) override;
    void draw(Renderer *rend) override;

    void setOnButtonPressedCallback(OnButtonPressedCallback callb) override;
};

#endif // DUMMYUIMANAGER_HPP
