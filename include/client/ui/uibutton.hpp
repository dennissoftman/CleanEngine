#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP

#include "client/ui/uilabel.hpp"
#include <boost/signals2.hpp>

class UIButton : public UILabel
{
public:
    UIButton();

    void clickSubscribe(const std::function<void ()> &callb);

    void onClick();

    virtual UIElement::UIType getType() const override;
private:
    boost::signals2::signal<void()> m_clickEvents;
};

#endif // UIBUTTON_HPP
