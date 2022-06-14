#ifndef UIELEMENT_HPP
#define UIELEMENT_HPP

#include <glm/glm.hpp>

class UIElement
{
public:
    virtual ~UIElement() {}

    virtual void draw() = 0;

    virtual void setPosition(const glm::vec2 &pos) = 0;
    virtual const glm::vec2 &getPosition() const = 0;

    virtual void setSize(const glm::vec2 &size) = 0;
    virtual const glm::vec2 &getSize() const = 0;

    enum UIType {
        eLabel,
        eButton,
    };

    virtual UIType getType() const = 0;
};

#endif // UIELEMENT_HPP
