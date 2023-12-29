#ifndef ANIMATION_PRIMITIVE_HPP
#define ANIMATION_PRIMITIVE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


class AnimationKeyFrame 
{
public:
    AnimationKeyFrame() = default;
    AnimationKeyFrame(const glm::mat4& trs, double time);
    AnimationKeyFrame(double time);

    void setTime(double t);
    double time() const;

    void setPosition(const glm::vec3& pos);
    const std::optional<glm::vec3>& position() const;

    void setRotation(const glm::quat& rot);
    const std::optional<glm::quat>& rotation() const;

    void setScale(const glm::vec3& scl);
    const std::optional<glm::vec3>& scale() const;

    glm::mat4 transform() const;
private:
    double m_time;
    std::optional<glm::vec3> m_position;
    std::optional<glm::quat> m_rotation;
    std::optional<glm::vec3> m_scale;
};

class AnimationPrimitive
{
public:
    AnimationPrimitive() = default;
    AnimationPrimitive(const std::string& name, double duration);

    void setName(const std::string& name);
    const std::string& name() const;

    void setDuration(double value);
    double duration() const;

    size_t channelCount() const;
    void setMeshIds(uint32_t channelId, const std::vector<uint32_t>& meshIds);
    const std::vector<uint32_t>& affectedMeshIds(uint32_t channelId) const;

    void addKeyframe(uint32_t channelId, AnimationKeyFrame keyFrame);
    AnimationKeyFrame keyframe(uint32_t channelId, double timecode) const;
private:
    std::string m_name;
    double m_duration;
    std::unordered_map<uint32_t, std::vector<AnimationKeyFrame>> m_keyframes;
    std::unordered_map<uint32_t, std::vector<uint32_t>> m_meshIdsPerChannel;
};

#endif