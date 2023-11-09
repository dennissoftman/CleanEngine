#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <memory>

class Renderer;
class Entity;

class Component {
public:
    Component(std::shared_ptr<Entity> parent);
    virtual ~Component();

    virtual void draw(Renderer *rend);
    virtual void update(double dt);
    std::shared_ptr<Entity> getParent();
    virtual bool isDrawable() const;

    virtual const char *getName() const;
protected:
    std::shared_ptr<Entity> m_parent;
};

#endif // COMPONENT_HPP
