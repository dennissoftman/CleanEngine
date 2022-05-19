#ifndef CAMERA3D_HPP
#define CAMERA3D_HPP

#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>

#define CAMERA_UP (glm::vec3(0, 1, 0))

namespace CamChangedFlagBits
{
    enum CamChangedFlagBits
    {
        eProjection=0x1,
        eView=0x2,
        eAll=~0
    };
}

class Camera3D
{
public:
    Camera3D(float fov=90.f, float aspect=16.f/9.f, float znear=0.1f, float zfar=1000.f);

    void move(const glm::vec3 &d);
    void rotate(float a, const glm::vec3 &axis);

    void setPitchConstraint(float _min, float _max);

    void setPosition(const glm::vec3 &pos);
    const glm::vec3 &getPosition() const;
    void setRotation(const glm::vec3 &rot);
    const glm::vec3 &getRotation() const;

    const glm::vec3 &frontVector();
    const glm::vec3 &rightVector();
    const glm::vec3 &upVector();

    void setFOV(float fov);
    void setAspect(float aspect);
    void setClipping(float near, float far);

    const glm::mat4 &getViewMatrix();
    const glm::mat4 &getProjectionMatrix();
private:
    void updateMatrices();

    int m_changedFlags; // dirty flag

    float m_fov, m_aspect, m_znear, m_zfar;
    glm::mat4 m_projectionMatrix, m_viewMatrix;
    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec2 m_pitchConstraint;

    glm::vec3 m_front, m_right, m_up;
};

#endif // CAMERA3D_HPP
