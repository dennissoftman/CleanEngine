#include "camera3d.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

static const char *MODULE_NAME = "Camera3D";

Camera3D::Camera3D(float fov, float aspect, float znear, float zfar)
    : m_fov(fov), m_aspect(aspect), m_znear(znear), m_zfar(zfar)
{
    m_changedFlags = -1; // all ones
}

void Camera3D::move(const glm::vec3 &d)
{
    m_position -= d;
    m_changedFlags |= CamChangedView;
}

void Camera3D::rotate(float a, const glm::vec3 &axis)
{
    m_rotation += axis * a;
    m_changedFlags |= CamChangedView;
}

void Camera3D::setPosition(const glm::vec3 &pos)
{
    m_position = pos;
    m_changedFlags |= CamChangedView;
}

void Camera3D::setRotation(const glm::vec3 &rot)
{
    m_rotation = rot;
}

void Camera3D::setFOV(float fov)
{
    m_fov = fov;
    m_changedFlags |= CamChangedProjection;
}

void Camera3D::setAspect(float aspect)
{
    m_aspect = aspect;
    m_changedFlags |= CamChangedProjection;
}

void Camera3D::setClipping(float near, float far)
{
    m_znear = std::min(near, far);
    m_zfar = std::max(near, far);
    m_changedFlags |= CamChangedProjection;
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
    if(m_changedFlags & CamChangedProjection)
    {
        m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspect, m_znear, m_zfar);
        m_changedFlags |= CamChangedView;
    }

    if(m_changedFlags & CamChangedView)
    {
        m_viewMatrix = glm::translate(glm::mat4(1), m_position) * glm::toMat4(glm::quat(m_rotation));
    }

    m_changedFlags = 0;
}
