#include <spdlog/spdlog.h>

#include "common/entities/animatedmodel.hpp"
#include "client/renderer.hpp"
#include "common/entities/entity.hpp"
#include "common/utils.hpp"


AnimatedModelComponent::AnimatedModelComponent(std::shared_ptr<Entity> parent)
    : StaticMesh(parent), m_time(0.0), m_looping(false), m_shouldPlay(true), m_currentAnimation(nullptr)
{
    m_name = "AnimatedModel_" + uuids::to_string(Utils::uuidGenerator());
}

AnimatedModelComponent::~AnimatedModelComponent()
{

}

void AnimatedModelComponent::draw(Renderer *rend)
{
    if(!m_visible)
        return;

    if (m_currentAnimation == nullptr)
        rend->queueRender(m_meshId, std::vector<size_t>{}, m_modelMatrix);
    else
        rend->queueRender(m_meshId, std::vector<size_t>{}, m_modelMatrix, m_meshTransforms);
}

void AnimatedModelComponent::update(double dt)
{
    const glm::vec3& pos = m_parent.get()->getPosition();
    const glm::quat& rot = m_parent.get()->getRotation();
    const glm::vec3& scale = m_parent.get()->getScale();

    m_modelMatrix = glm::translate(glm::mat4(1.f), pos) *
                    glm::toMat4(rot) *
                    glm::scale(glm::mat4(1.f), scale);

    if (m_currentAnimation == nullptr || !m_shouldPlay)
        return;

    for (uint32_t channelId = 0; channelId < m_currentAnimation->channelCount(); channelId++)
    {
        const auto& affectedMeshIds = m_currentAnimation->affectedMeshIds(channelId);

        auto keyframe = m_currentAnimation->keyframe(channelId, m_time);
        uint32_t transformsCount = (*std::max_element(affectedMeshIds.begin(), affectedMeshIds.end())) + 1; // transformId is 0-indexed, so we add 1
        m_meshTransforms.resize(std::max(static_cast<size_t>(transformsCount), m_meshTransforms.size()), glm::mat4(1.f));
        for (uint32_t meshId : affectedMeshIds)
        {
            m_meshTransforms[meshId] = keyframe.transform();
        }
    }

    m_time += dt;
    if (m_time >= m_currentAnimation->duration())
    {
        if (m_looping)
            m_time = 0.0;
        else
            m_shouldPlay = false;
    }
}

void AnimatedModelComponent::setAnimation(std::shared_ptr<AnimationPrimitive> anim)
{
    if (m_currentAnimation != anim)
    {
        m_currentAnimation = anim;
        m_time = 0.0;
        if (m_looping)
            m_shouldPlay = true;
        else
            m_shouldPlay = false;
    }
}

bool AnimatedModelComponent::trigger()
{
    if (m_currentAnimation == nullptr)
        return false;
    if ((m_time < m_currentAnimation->duration()) && m_shouldPlay)
        return false;

    m_time = 0.0;
    m_shouldPlay = true;
    return true;
}

void AnimatedModelComponent::setLooping(bool yes)
{
    m_looping = yes;
}

double AnimatedModelComponent::time() const
{
    return m_time;
}

const char *AnimatedModelComponent::getName() const
{
    return m_name.c_str();
}