#include "common/entities/entity.hpp"
#include "common/utils.hpp"


Entity::Entity()
    : Entity(Utils::uuidGenerator.getUUID())
{
}

Entity::Entity(const UUIDv4::UUID &id)
    : m_id(id),
      m_visible(true),
      m_position(glm::vec3(0.f)),
      m_rotation(glm::quat(glm::vec3(0.f))),
      m_scale(glm::vec3(1.f))
{
}

Entity::~Entity()
{
    m_components.clear();
}

void Entity::draw(Renderer *rend)
{
    if(!m_visible)
        return;

    for(auto it = m_components.begin(); it != m_components.end(); it++)
    {
        if(it->second->isDrawable()) {
            it->second->draw(rend);
        }
    }
}

void Entity::update(double dt)
{
    for(auto it = m_components.begin(); it != m_components.end(); it++)
    {
        it->second->update(dt);
    }
    m_updateEvents(this, dt);
}

UUIDv4::UUID Entity::getID() const
{
    return m_id;
}

void Entity::setVisible(bool yes)
{
    m_visible = yes;
}

void Entity::destroy()
{
    m_destroyEvents(this);
}

void Entity::setPosition(const glm::vec3 &pos)
{
    m_position = pos;
}

void Entity::setRotation(const glm::quat &qrot)
{
    m_rotation = qrot;
}

void Entity::setEulerRotation(const glm::vec3 &rot)
{
    setRotation(glm::quat(rot));
}

void Entity::setScale(const glm::vec3 &scale)
{
    m_scale = scale;
}

const glm::vec3 &Entity::getPosition() const
{
    return m_position;
}

glm::vec3 Entity::getEulerRotation() const
{
    return glm::eulerAngles(m_rotation);
}

const glm::quat &Entity::getRotation() const
{
    return m_rotation;
}

const glm::vec3 &Entity::getScale() const
{
    return m_scale;
}

void Entity::setScene(Scene3D *parent)
{
    m_parentScene = parent;
}

Scene3D *Entity::getParentScene() const
{
    return m_parentScene;
}

void Entity::attachComponent(std::shared_ptr<Component> comp)
{
    if(m_components.find(comp->getName()) == m_components.end())
        m_components[comp->getName()] = comp;
}

void Entity::removeComponent(const char *name)
{
    m_components.erase(name);
}

void Entity::updateSubscribe(const std::function<void (Entity *, double)> &callb)
{
    m_updateEvents.connect(callb);
}

void Entity::destroySubscribe(const std::function<void (Entity *)> &callb)
{
    m_destroyEvents.connect(callb);
}
