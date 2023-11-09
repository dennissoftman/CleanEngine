#include "common/entities/bulletbodycomponent.hpp"
#include "common/entities/entity.hpp"
#include "common/servicelocator.hpp"
#include "server/bulletphysicsmanager.hpp"
#include <glm/gtx/quaternion.hpp>


std::shared_ptr<BodyComponent> BodyComponent::createComponent(const PhysicsBodyCreateInfo& cInfo, std::shared_ptr<Entity> parent)
{
    return std::make_shared<BulletBodyComponent>(cInfo, parent);
}

BulletBodyComponent::BulletBodyComponent(const PhysicsBodyCreateInfo& cInfo, std::shared_ptr<Entity> parent)
    : BodyComponent(cInfo, parent),
      m_isDirty(true)
{
    auto physmgr = static_cast<BulletPhysicsManager*>(&ServiceLocator::getPhysicsManager());
    const glm::vec3& pos = parent->getPosition();
    const glm::quat& rot = parent->getRotation();
    m_body = static_cast<btRigidBody*>(physmgr->createBody(m_bodyInfo, pos, rot));
    m_body->setUserPointer(this);
    m_position = btVector3(pos.x, pos.y, pos.z);
    m_rotation = btQuaternion(rot.x, rot.y, rot.z, rot.w);
}

BulletBodyComponent::~BulletBodyComponent()
{
    m_body->setUserPointer(nullptr);
}

void BulletBodyComponent::draw(Renderer *rend)
{

}

void BulletBodyComponent::update(double dt)
{
    if(!m_body)
        return;

    if(m_isDirty)
    {
        Entity *ent = m_parent.get();
        ent->setPosition(glm::vec3(m_position.x(), m_position.y(), m_position.z()));
        ent->setRotation(glm::quat(m_rotation.w(), m_rotation.x(), m_rotation.y(), m_rotation.z()));
        m_isDirty = false;
    }
}

const char *BulletBodyComponent::getName() const
{
    return "BulletBodyComponent";
}

bool BulletBodyComponent::isDrawable() const
{
    return false;
}

void BulletBodyComponent::updateTransform(const btVector3 &pos, const btQuaternion &rot)
{
    m_position = pos;
    m_rotation = rot;
    m_isDirty = true;
}
