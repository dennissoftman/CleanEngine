#include "client/inputmanager.hpp"

InputManager::InputManager()
{

}

void InputManager::keyboardCallback(int key, int scancode, int action, int mods)
{
    m_keyEvents(key, scancode, action, mods);
}

void InputManager::keyboardSubscribe(const clean::key_callback &slot)
{
    m_keyEvents.connect(slot);
}

void InputManager::mouseButtonCallback(int button, int action)
{
    m_mouseBtnEvents(button, action);
}

void InputManager::mouseButtonSubscribe(const clean::mouse_button_callback &callb)
{
    m_mouseBtnEvents.connect(callb);
}

void InputManager::mousePositionCallback(int mx, int my)
{
    m_mousePosEvents(mx, my);
}

void InputManager::mousePositionSubscribe(const clean::mouse_pos_callback &callb)
{
    m_mousePosEvents.connect(callb);
}

void InputManager::mouseScrollCallback(int sx, int sy)
{
    m_mouseScrollEvents(sx, sy);
}

void InputManager::mouseScrollSubscribe(const clean::mouse_scroll_callback &callb)
{
    m_mouseScrollEvents.connect(callb);
}
