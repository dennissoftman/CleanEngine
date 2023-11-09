#ifndef BULLETBODYCOMPONENT_HPP
#define BULLETBODYCOMPONENT_HPP

#include "common/entities/bodycomponent.hpp"
#include "BulletDynamics/Dynamics/btRigidBody.h"

class BulletBodyComponent : public BodyComponent {
public:
    BulletBodyComponent(const PhysicsBodyCreateInfo &cInfo, std::shared_ptr<Entity> parent);
    ~BulletBodyComponent();

    void draw(Renderer *rend) override;
    void update(double dt) override;

    const char *getName() const override;
    bool isDrawable() const override;

    void updateTransform(const btVector3 &pos, const btQuaternion &rot);

private:
    bool m_isDirty;
    btRigidBody *m_body;
    btVector3 m_position;
    btQuaternion m_rotation;
};

#endif // BODYCOMPONENT_HPP
