#include "common/eventmanager.hpp"

EventManager::EventManager()
{

}

void EventManager::update(double dt)
{
    m_updateEvents(dt);
}

void EventManager::updateSubscribe(const std::function<void (double)> &callb)
{
    m_updateEvents.connect(callb);
}

void EventManager::keyboardCallback(int key, int scancode, int action, int mods)
{
    m_keyEvents(key, scancode, action, mods);
}

void EventManager::keyboardSubscribe(const clean::key_callback &slot)
{
    m_keyEvents.connect(slot);
}

void EventManager::mouseButtonCallback(int button, int action)
{
    m_mouseBtnEvents(button, action);
}

void EventManager::mouseButtonSubscribe(const clean::mouse_button_callback &callb)
{
    m_mouseBtnEvents.connect(callb);
}

void EventManager::mousePositionCallback(int mx, int my)
{
    m_mousePosEvents(mx, my);
}

void EventManager::mousePositionSubscribe(const clean::mouse_pos_callback &callb)
{
    m_mousePosEvents.connect(callb);
}

void EventManager::mouseScrollCallback(int sx, int sy)
{
    m_mouseScrollEvents(sx, sy);
}

void EventManager::mouseScrollSubscribe(const clean::mouse_scroll_callback &callb)
{
    m_mouseScrollEvents.connect(callb);
}
