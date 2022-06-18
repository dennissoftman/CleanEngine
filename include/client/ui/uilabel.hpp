#ifndef UILABEL_HPP
#define UILABEL_HPP

#include "client/ui/uielement.hpp"
#include <string>
#include <functional>

class UILabel : public UIElement
{
public:
    UILabel();
    ~UILabel() override;

    void setText(const std::string &txt);
    const std::string &text() const;

    void setPosition(const glm::vec2 &pos) override;
    const glm::vec2 &getPosition() const override;
    void setSize(const glm::vec2 &size) override;
    const glm::vec2 &getSize() const override;

    virtual UIElement::UIType getType() const override;
protected:
    glm::vec2 m_pos, m_size;
    std::string m_text;
};

#endif // UILABEL_HPP
