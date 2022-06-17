#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include <functional>
#include <boost/signals2.hpp>

namespace clean
{
    enum KeyState : int
    {
        KeyReleased=-1,
        KeyPressed=1,
    };

    enum MouseState : int
    {
        MouseReleased=-1,
        MousePressed=1,
    };

    typedef std::function<void(int, int, int, int)> key_callback;
    typedef std::function<void(int, int)> mouse_pos_callback;
    typedef std::function<void(int, int)> mouse_scroll_callback;
    typedef std::function<void(int, int)> mouse_button_callback;
}

class InputManager
{
public:
    InputManager();

    /*
     * key - virtual mapped key
     * scancode - physical key number
     * action - pressed or released
     * mods - alt'ed, shift'ed, ctrl'ed, etc.
    */
    void keyboardCallback(int key, int scancode, int action, int mods);
    void keyboardSubscribe(const clean::key_callback &slot);

    /*
     * button - mouse button
     * action - pressed or released
    */
    void mouseButtonCallback(int button, int action);
    void mouseButtonSubscribe(const clean::mouse_button_callback &callb);

    /*
     * mx - mouse position x in window
     * my - mouse position y in window
    */
    void mousePositionCallback(int mx, int my);
    void mousePositionSubscribe(const clean::mouse_pos_callback &callb);

    /*
     * sx - scroll amount in x
     * sy - scroll amount in y
    */
    void mouseScrollCallback(int sx, int sy);
    void mouseScrollSubscribe(const clean::mouse_scroll_callback &callb);

    // TODO: joystick, etc.
private:
    boost::signals2::signal<void(double)> m_updateEvents;

    boost::signals2::signal<void(int, int, int, int)> m_keyEvents;
    boost::signals2::signal<void(int, int)> m_mouseBtnEvents;
    boost::signals2::signal<void(int, int)> m_mousePosEvents;
    boost::signals2::signal<void(int, int)> m_mouseScrollEvents;
};

#endif // EVENTMANAGER_HPP
