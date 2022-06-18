#include "client/ui/uispinbox.hpp"
#include <limits>

UISpinBox::UISpinBox()
    : m_label("value"),
      m_value(0),
      m_min(-1000.f),
      m_max(1000.f)
{

}

void UISpinBox::setMinimum(float a)
{
    m_min = std::min(a, m_max);
}

float UISpinBox::getMinimum() const
{
    return m_min;
}

void UISpinBox::setMaximum(float a)
{
    m_max = std::max(a, m_min);
}

float UISpinBox::getMaximum() const
{
    return m_max;
}

void UISpinBox::setValue(float a)
{
    m_value = std::max(std::min(a, m_max), m_min);
}

float UISpinBox::getValue() const
{
    return m_value;
}

void UISpinBox::setLabel(const std::string &lbl)
{
    m_label = lbl;
}

const std::string &UISpinBox::getLabel() const
{
    return m_label;
}

void UISpinBox::setPosition(const glm::vec2 &pos)
{
    m_pos = pos;
}

const glm::vec2 &UISpinBox::getPosition() const
{
    return m_pos;
}

void UISpinBox::setSize(const glm::vec2 &size)
{
    m_size = size;
}

const glm::vec2 &UISpinBox::getSize() const
{
    return m_size;
}

UIElement::UIType UISpinBox::getType() const
{
    return UIElement::UIType::eSpinBox;
}

void UISpinBox::changeValueSubscribe(const std::function<void (float)> &callb)
{
    m_changeValueEvents.connect(callb);
}

void UISpinBox::onChangeValue(float a)
{
    setValue(a);
    m_changeValueEvents(a);
}
