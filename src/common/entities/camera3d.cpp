#include "common/entities/camera3d.hpp"
#include "common/servicelocator.hpp"
#include "common/utils.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>

static const char *MODULE_NAME = "Camera3D";

Camera3D::Camera3D(float fov, float aspect, float znear, float zfar)
    : Entity(), m_fov(fov), m_aspect(aspect), m_znear(znear), m_zfar(zfar)
{
    m_viewMatrix = glm::mat4(1);
    m_projectionMatrix = glm::mat4(1);
    m_id = Utils::uuidGenerator();
}

void Camera3D::setPosition(const glm::vec3 &pos)
{
    Entity::setPosition(pos);
    m_viewMatrix.set_dirty();
}

const glm::vec3 &Camera3D::frontVector()
{
    if(m_viewMatrix.is_dirty())
        updateMatrices();
    return m_front;
}

const glm::vec3 &Camera3D::rightVector()
{
    if(m_viewMatrix.is_dirty())
        updateMatrices();
    return m_right;
}

const glm::vec3 &Camera3D::upVector()
{
    if(m_viewMatrix.is_dirty())
        updateMatrices();
    return m_up;
}

void Camera3D::setFOV(float fov)
{
    m_fov = fov;
    m_projectionMatrix.set_dirty();
}

void Camera3D::setAspect(float aspect)
{
    m_aspect = aspect;
    m_projectionMatrix.set_dirty();
}

void Camera3D::setClipping(float near, float far)
{
    m_znear = std::min(near, far);
    m_zfar = std::max(near, far);
    m_projectionMatrix.set_dirty();
}

const glm::mat4 &Camera3D::getViewMatrix()
{
    if(m_viewMatrix.is_dirty())
        updateMatrices();
    return m_viewMatrix.value();
}

const glm::mat4 &Camera3D::getProjectionMatrix()
{
    if(m_projectionMatrix.is_dirty())
        updateMatrices();
    return m_projectionMatrix.value();
}

void Camera3D::updateMatrices()
{
    if(m_projectionMatrix.is_dirty())
    {
        m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspect, m_znear, m_zfar);
        m_projectionMatrix.clear_dirty();
    }

    if(m_viewMatrix.is_dirty())
    {
        m_right = glm::normalize(m_rotation * glm::vec3(1, 0, 0));
        m_up    = glm::normalize(m_rotation * glm::vec3(0, 1, 0));
        m_front = glm::normalize(m_rotation * glm::vec3(0, 0, 1));

        m_viewMatrix = glm::lookAt(m_position,
                                   m_position + m_front,
                                   m_up);
        m_viewMatrix.clear_dirty();

        ServiceLocator::getAudioManager().updateListener(m_position, glm::vec3(0,0,0), m_front, m_up);
    }
}

void Camera3D::setVisible(bool yes)
{

}

Camera3D &Camera3D::operator =(const Camera3D &other)
{
    m_parentScene = other.m_parentScene;

    m_fov = other.m_fov;
    m_aspect = other.m_aspect;
    m_znear = other.m_znear;
    m_zfar = other.m_zfar;

    m_projectionMatrix = other.m_projectionMatrix;
    m_viewMatrix = other.m_projectionMatrix;
    m_position = other.m_position;
    m_rotation = other.m_rotation;
    m_scale = other.m_scale;

    m_projectionMatrix.set_dirty();
    m_viewMatrix.set_dirty();

    return *this;
}

void Camera3D::setRotation(const glm::quat &qrot)
{
    Entity::setRotation(qrot);
    m_viewMatrix.set_dirty();
}

void Camera3D::setEulerRotation(const glm::vec3 &rot)
{
    setRotation(glm::quat(rot));
}

void Camera3D::setScale(const glm::vec3 &scale)
{

}
