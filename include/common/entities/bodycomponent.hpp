#ifndef BODYCOMPONENT_HPP
#define BODYCOMPONENT_HPP

#include "common/entities/component.hpp"
#include "server/physicsmanager.hpp"

class BodyComponent : public Component {
public:
    static std::shared_ptr<BodyComponent> createComponent(const PhysicsBodyCreateInfo &cInfo, std::shared_ptr<Entity> parent);

    BodyComponent(const PhysicsBodyCreateInfo &cInfo, std::shared_ptr<Entity> parent);

    virtual void draw(Renderer *rend) override;
    virtual void update(double dt) override;

    virtual bool isDrawable() const override;
    virtual const char *getName() const override;

protected:
    PhysicsBodyCreateInfo m_bodyInfo;
};

#endif // BODYCOMPONENT_HPP
