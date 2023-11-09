#include "client/inputmanager.hpp"
#include <GLFW/glfw3.h>

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

void InputManager::joystickEventCallback(int jid, int event)
{
    if(event == GLFW_CONNECTED)
    {
        if(m_connectedJoysticks.find(jid) == m_connectedJoysticks.end())
            m_connectedJoysticks[jid] = JoystickState{};

        JoystickState state = m_connectedJoysticks[jid];
        state.isConnected = true;
        if(glfwJoystickIsGamepad(jid) == GLFW_TRUE)
            state.isGamepad = true;
        else
            state.isGamepad = false;

        m_connectedJoysticks[jid] = state;
        fprintf(stderr, "Joystick %d is connected\n", jid);
    }
    else if(event == GLFW_DISCONNECTED)
    {
        m_connectedJoysticks.erase(jid);
        fprintf(stderr, "Joystick %d is disconnected\n", jid);
    }
    else
    {
        fprintf(stderr, "Unknown event id: %d\n", event);
    }
}

void InputManager::joystickEventSubscribe(const clean::joystick_event_callback &callb)
{

}
