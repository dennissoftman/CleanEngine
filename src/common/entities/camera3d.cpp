#include "common/entities/camera3d.hpp"
#include "common/servicelocator.hpp"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>

static const char *MODULE_NAME = "Camera3D";

Camera3D::Camera3D(float fov, float aspect, float znear, float zfar)
    : m_fov(fov), m_aspect(aspect), m_znear(znear), m_zfar(zfar),
      m_position(glm::vec3()), m_rotation(glm::vec3()), m_scale(glm::vec3(1,1,1)),
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
    m_rotation = glm::rotate(m_rotation, a, axis);
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

glm::vec3 Camera3D::getEulerRotation() const
{
    return glm::eulerAngles(m_rotation);
}

const glm::quat &Camera3D::getRotation() const
{
    return m_rotation;
}

const glm::vec3 &Camera3D::getScale() const
{
    return m_scale;
}

void Camera3D::setScene(Scene3D *parent)
{
    m_parentScene = parent;
}

Scene3D *Camera3D::getParentScene() const
{
    return m_parentScene;
}

const char *Camera3D::getType()
{
    return "Camera3D";
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
        glm::vec3 front = glm::vec3(m_rotation.x,
                                    m_rotation.y,
                                    m_rotation.z);

        m_front = glm::normalize(front);
        m_right = glm::normalize(glm::cross(m_front, CAMERA_UP));
        m_up    = glm::normalize(glm::cross(m_right, m_front));

        m_viewMatrix = glm::lookAt(m_position,
                                   m_position + m_front,
                                   m_up);

        ServiceLocator::getAudioManager().updateListener(m_position, glm::vec3(0,0,0), m_front, m_up);
    }

    m_changedFlags = 0;
}

void Camera3D::draw(Renderer *rend)
{
    (void)rend;
}

void Camera3D::update(double dt)
{
    (void)dt;
}

void Camera3D::setVisible(bool yes)
{
    (void)yes;
}

void Camera3D::destroy()
{

}

void Camera3D::setRotation(const glm::quat &qrot)
{
    m_rotation = qrot;
    m_changedFlags |= CamChangedFlagBits::eView;
}

void Camera3D::setEulerRotation(const glm::vec3 &rot)
{
    setRotation(glm::quat(rot));
}

void Camera3D::setScale(const glm::vec3 &scale)
{
    (void)scale;
}