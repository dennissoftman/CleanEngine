#ifndef UITEXTINPUT_HPP
#define UITEXTINPUT_HPP

#include "client/ui/uielement.hpp"

#include <boost/signals2.hpp>

class UITextInput : public UIElement
{
public:
    UITextInput();

    void setPosition(const glm::vec2 &pos);
    const glm::vec2 &getPosition() const;
    void setSize(const glm::vec2 &size);
    const glm::vec2 &getSize() const;

    void setLabel(const std::string &lbl);
    const std::string &getLabel() const;

    void setValue(const std::string& val);
    const std::string& getValue() const;

    UIType getType() const;

    // events
    void changeValueSubscribe(const std::function<void(const std::string&)> &callb);
    void onChangeValue(const std::string& val);
protected:
    boost::signals2::signal<void(const std::string&)> m_changeValueEvents;

    std::string m_label;
    std::string m_value;
    glm::vec2 m_pos, m_size;
};

#endif // UITEXTINPUT_HPP
