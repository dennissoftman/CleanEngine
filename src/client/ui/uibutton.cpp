#include "client/ui/uibutton.hpp"

UIButton::UIButton()
{

}

void UIButton::clickSubscribe(const std::function<void ()> &callb)
{
    m_clickEvents.connect(callb);
}

void UIButton::onClick()
{
    m_clickEvents();
}

UIElement::UIType UIButton::getType() const
{
    return UIElement::UIType::eButton;
}
