#include "common/entities/bodycomponent.hpp"

BodyComponent::BodyComponent(const PhysicsBodyCreateInfo &cInfo, std::shared_ptr<Entity> parent)
    : Component(parent), m_bodyInfo(cInfo)
{

}

void BodyComponent::draw(Renderer *rend)
{

}

void BodyComponent::update(double dt)
{

}

bool BodyComponent::isDrawable() const
{
    return false;
}

const char *BodyComponent::getName() const
{
    return "BodyComponent";
}
