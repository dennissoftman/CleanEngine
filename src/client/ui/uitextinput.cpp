#include "client/ui/uitextinput.hpp"

UITextInput::UITextInput()
    : m_label("textinput")
{

}

void UITextInput::setPosition(const glm::vec2 &pos)
{
    m_pos = pos;
}

const glm::vec2 &UITextInput::getPosition() const
{
    return m_pos;
}

void UITextInput::setSize(const glm::vec2 &size)
{
    m_size = size;
}

const glm::vec2 &UITextInput::getSize() const
{
    return m_size;
}

void UITextInput::setLabel(const std::string &lbl)
{
    m_label = lbl;
}

const std::string &UITextInput::getLabel() const
{
    return m_label;
}

void UITextInput::setValue(const std::string &val)
{
    m_value = val;
}

const std::string &UITextInput::getValue() const
{
    return m_value;
}

UIElement::UIType UITextInput::getType() const
{
    return eTextInput;
}

void UITextInput::changeValueSubscribe(const std::function<void (const std::string &)> &callb)
{
    m_changeValueEvents.connect(callb);
}

void UITextInput::onChangeValue(const std::string &val)
{
    setValue(val);
    m_changeValueEvents(val);
}
