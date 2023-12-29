#include "common/entities/component.hpp"
#include "common/entities/entity.hpp"

Component::Component(std::shared_ptr<Entity> parent)
    : m_parent(parent)
{
    
}

Component::~Component()
{

}

std::shared_ptr<Entity> Component::getParent() const
{
    return m_parent;
}

const char *Component::getName() const
{
    return "Component";
}