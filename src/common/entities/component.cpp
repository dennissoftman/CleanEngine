#include "common/entities/component.hpp"
#include "common/entities/entity.hpp"

Component::Component(std::shared_ptr<Entity> parent)
    : m_parent(parent)
{

}

Component::~Component()
{
    // cleanup
}

void Component::draw(Renderer *rend)
{

}

void Component::update(double dt)
{
    // DO NOTHING
}

std::shared_ptr<Entity> Component::getParent()
{
    return m_parent;
}

bool Component::isDrawable() const
{
    return false;
}

const char *Component::getName() const
{
    return "Component"; // name for abstract component (dummy)
}
