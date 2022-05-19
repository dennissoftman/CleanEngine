#include "camera3d.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>

static const char *MODULE_NAME = "Camera3D";

Camera3D::Camera3D(float fov, float aspect, float znear, float zfar)
    : m_fov(fov), m_aspect(aspect), m_znear(znear), m_zfar(zfar),
      m_position(glm::vec3()), m_rotation(glm::vec3()),
      m_pitchConstraint(-INFINITY, INFINITY)
{
    m_changedFlags = CamChangedFlagBits::eAll; // all ones
}

void Camera3D::move(const glm::vec3 &d)
{
    m_position += d;
    m_changedFlags |= CamChangedFlagBits::eView;
}

void Camera3D::rotate(float a, const glm::vec3 &axis)
{
    m_rotation += a * axis;
    if(m_rotation.x < m_pitchConstraint.x) // min
        m_rotation.x = m_pitchConstraint.x;
    if(m_rotation.x > m_pitchConstraint.y) // max
        m_rotation.x = m_pitchConstraint.y;

    m_changedFlags |= CamChangedFlagBits::eView;
}

void Camera3D::setPitchConstraint(float _min, float _max)
{
    m_pitchConstraint = glm::vec2(_min, _max);
}

void Camera3D::setPosition(const glm::vec3 &pos)
{
    m_position = pos;
    m_changedFlags |= CamChangedFlagBits::eView;
}

const glm::vec3 &Camera3D::getPosition() const
{
    return m_position;
}

void Camera3D::setRotation(const glm::vec3 &rot)
{
    m_rotation = rot;
    m_changedFlags |= CamChangedFlagBits::eView;
}

const glm::vec3 &Camera3D::getRotation() const
{
    return m_rotation;
}

const glm::vec3 &Camera3D::frontVector()
{
    if(m_changedFlags)
        updateMatrices();
    return m_front;
}

const glm::vec3 &Camera3D::rightVector()
{
    if(m_changedFlags)
        updateMatrices();
    return m_right;
}

const glm::vec3 &Camera3D::upVector()
{
    if(m_changedFlags)
        updateMatrices();
    return m_up;
}

void Camera3D::setFOV(float fov)
{
    m_fov = fov;
    m_changedFlags |= CamChangedFlagBits::eProjection;
}

void Camera3D::setAspect(float aspect)
{
    m_aspect = aspect;
    m_changedFlags |= CamChangedFlagBits::eProjection;
}

void Camera3D::setClipping(float near, float far)
{
    m_znear = std::min(near, far);
    m_zfar = std::max(near, far);
    m_changedFlags |= CamChangedFlagBits::eProjection;
}

const glm::mat4 &Camera3D::getViewMatrix()
{
    if(m_changedFlags)
        updateMatrices();
    return m_viewMatrix;
}

const glm::mat4 &Camera3D::getProjectionMatrix()
{
    if(m_changedFlags)
        updateMatrices();
    return m_projectionMatrix;
}

void Camera3D::updateMatrices()
{
    if(m_changedFlags & CamChangedFlagBits::eProjection)
    {
        m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspect, m_znear, m_zfar);
        m_changedFlags |= CamChangedFlagBits::eView;
    }

    if(m_changedFlags & CamChangedFlagBits::eView)
    {
        glm::vec3 front;
        front.x = glm::cos(m_rotation.y) * glm::cos(m_rotation.x);
        front.y = glm::sin(m_rotation.x);
        front.z = glm::sin(m_rotation.y) * glm::cos(m_rotation.x);

        m_front = glm::normalize(front);
        m_right = glm::normalize(glm::cross(m_front, CAMERA_UP));
        m_up    = glm::normalize(glm::cross(m_right, m_front));

        m_viewMatrix = glm::lookAt(m_position,
                                   m_position + m_front,
                                   m_up);
    }

    m_changedFlags = 0;
}
