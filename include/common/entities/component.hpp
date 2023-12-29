#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <memory>

class Renderer;
class Entity;

class Component {
public:
    Component(std::shared_ptr<Entity> parent);
    virtual ~Component();

    virtual void draw(Renderer *rend) = 0;
    virtual void update(double dt) = 0;
    std::shared_ptr<Entity> getParent() const;
    virtual bool isDrawable() const = 0;

    virtual const char *getName() const;
protected:
    std::shared_ptr<Entity> m_parent;
};

#endif // COMPONENT_HPP
