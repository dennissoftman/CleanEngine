#include "client/ui/uilabel.hpp"
#include <functional>

UILabel::UILabel()
{
    m_drawCallback = [](const char*, const glm::vec2&){  };
}

UILabel::~UILabel()
{

}

void UILabel::setText(const std::string &txt)
{
    m_text = txt;
}

const std::string &UILabel::text() const
{
    return m_text;
}

void UILabel::setPosition(const glm::vec2 &pos)
{
    m_pos = pos;
}

const glm::vec2 &UILabel::getPosition() const
{
    return m_pos;
}

void UILabel::setSize(const glm::vec2 &size)
{
    m_size = size;
}

const glm::vec2 &UILabel::getSize() const
{
    return m_size;
}

UIElement::UIType UILabel::getType() const
{
    return UIElement::eLabel;
}
