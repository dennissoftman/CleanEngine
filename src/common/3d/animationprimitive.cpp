#include <stdexcept>
#include <spdlog/spdlog.h>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/matrix_interpolation.hpp>

#include "common/3d/animationprimitive.hpp"


AnimationPrimitive::AnimationPrimitive(const std::string& name, double duration)
    : m_name(name), m_duration(duration)
{}

void AnimationPrimitive::setName(const std::string& name)
{
    m_name = name;
}

const std::string& AnimationPrimitive::name() const
{
    return m_name;
}

void AnimationPrimitive::setDuration(double value)
{
    m_duration = value;
}

double AnimationPrimitive::duration() const
{
    return m_duration;
}

size_t AnimationPrimitive::channelCount() const
{
    return m_keyframes.size();
}

void AnimationPrimitive::setMeshIds(uint32_t channelId, const std::vector<uint32_t>& meshIds)
{
    m_meshIdsPerChannel.emplace(channelId, meshIds);
}

const std::vector<uint32_t>& AnimationPrimitive::affectedMeshIds(uint32_t channelId) const
{
    auto meshIdsIt = m_meshIdsPerChannel.find(channelId);
    if (meshIdsIt == m_meshIdsPerChannel.end())
        throw std::runtime_error(fmt::format("Model has no channelID {}", channelId));
    return meshIdsIt->second;
}

void AnimationPrimitive::addKeyframe(uint32_t channelId, AnimationKeyFrame keyFrame)
{
    auto channelIt = m_keyframes.find(channelId);
    if (channelIt == m_keyframes.end()) // new channel
    {
        m_keyframes.emplace(channelId, std::vector<AnimationKeyFrame>{keyFrame});
    }
    else // existing channel
    {
        channelIt->second.push_back(keyFrame);
    }
}

AnimationKeyFrame AnimationPrimitive::keyframe(uint32_t channelId, double timecode) const
{
    auto channelIt = m_keyframes.find(channelId);
    if(channelIt == m_keyframes.end())
        return AnimationKeyFrame();

    const auto& keyframes = channelIt->second;
    if (keyframes.size() == 0)
        return AnimationKeyFrame();
    if (timecode <= keyframes[0].time())
    {
        return keyframes[0];
    }
    if (timecode >= keyframes.back().time())
    {
        return keyframes.back();
    }

    AnimationKeyFrame result;
    for(size_t i = 1; i < keyframes.size(); i++)
    {
        const auto& kf0 = keyframes.at(i-1);
        const auto& kf1 = keyframes.at(i);

        if (timecode >= kf0.time() && timecode < kf1.time())
        {
            float dt = static_cast<float>((timecode - kf0.time()) / (kf1.time() - kf0.time()));

            glm::vec3 pos0, pos1;
            glm::quat rot0, rot1;
            glm::vec3 scl0, scl1;
            glm::vec3 t0;
            glm::vec4 t1;
            glm::decompose(kf0.transform(), scl0, rot0, pos0, t0, t1);
            glm::decompose(kf1.transform(), scl1, rot1, pos1, t0, t1);

            glm::vec3 pos = glm::mix(pos0, pos1, dt);
            glm::quat rot = glm::slerp(rot0, rot1, dt);
            glm::vec3 scl = glm::mix(scl0, scl1, dt);

            glm::mat4 trs = glm::translate(glm::mat4(1.f), pos) * glm::toMat4(rot) * glm::scale(glm::mat4(1.f), scl);
            result = AnimationKeyFrame(trs, timecode);
            break;
        }
    }
    return result;
}

AnimationKeyFrame::AnimationKeyFrame(const glm::mat4& trs, double time)
    : m_time(time)
{
    glm::vec3 pos;
    glm::quat rot;
    glm::vec3 scl;
    
    pos = trs[3];
    for (int i = 0; i < 3; i++)
        scl[i] = glm::length(glm::vec3(trs[i]));
    const glm::mat3 rotMtx(
        glm::vec3(trs[0]) / scl[0],
        glm::vec3(trs[1]) / scl[1],
        glm::vec3(trs[2]) / scl[2]);
    rot = glm::quat_cast(rotMtx);

    m_position = pos;
    m_rotation = rot;
    m_scale    = scl;
}

AnimationKeyFrame::AnimationKeyFrame(double time)
    : m_time(time)
{}

void AnimationKeyFrame::setTime(double t)
{
    m_time = t;
}

double AnimationKeyFrame::time() const
{
    return m_time;
}

void AnimationKeyFrame::setPosition(const glm::vec3& pos)
{
    m_position = pos;
}

const std::optional<glm::vec3>& AnimationKeyFrame::position() const
{
    return m_position;
}

void AnimationKeyFrame::setRotation(const glm::quat& rot)
{
    m_rotation = rot;
}

const std::optional<glm::quat>& AnimationKeyFrame::rotation() const
{
    return m_rotation;
}

void AnimationKeyFrame::setScale(const glm::vec3& scl)
{
    m_scale = scl;
}

const std::optional<glm::vec3>& AnimationKeyFrame::scale() const
{
    return m_scale;
}

glm::mat4 AnimationKeyFrame::transform() const
{
    glm::mat4 result(1.f); // will contain TRS transformation matrix
    if (m_position.has_value())
        result = glm::translate(result, m_position.value());
    if (m_rotation.has_value())
        result *= glm::toMat4(m_rotation.value());
    if (m_scale.has_value())
        result = glm::scale(result, m_scale.value());

    return result;
}
