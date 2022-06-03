#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include "client/renderer.hpp"

struct ButtonData
{
    int id; // button identifier
};

typedef void(*OnButtonPressedCallback)(const ButtonData &);

class UIManager
{
    friend class Renderer;
public:
    virtual ~UIManager() {}

    virtual void init() = 0;

    virtual void update(double dt) = 0;
    virtual void draw() =  0;

    virtual void setOnButtonPressedCallback(OnButtonPressedCallback callb) = 0;

    static UIManager *create();
};

#endif // UIMANAGER_HPP
