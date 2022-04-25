#ifndef CAMERA3D_HPP
#define CAMERA3D_HPP

#include <glm/mat4x4.hpp>

#define CAMERA_UP (glm::vec3(0, 1, 0))

enum CamChangedFlag
{
    CamChangedProjection=0x1,
    CamChangedView=0x2
};

class Camera3D
{
public:
    Camera3D(float fov=90.f, float aspect=16.f/9.f, float znear=0.1f, float zfar=1000.f);

    void move(const glm::vec3 &d);
    void rotate(float a, const glm::vec3 &axis);

    void setPosition(const glm::vec3 &pos);
    void setRotation(const glm::vec3 &rot);

    void setFOV(float fov);
    void setAspect(float aspect);
    void setClipping(float near, float far);

    const glm::mat4 &getViewMatrix();
    const glm::mat4 &getProjectionMatrix();
private:
    void updateMatrices();

    uint8_t m_changedFlags; // dirty flag

    float m_fov, m_aspect, m_znear, m_zfar;
    glm::mat4 m_projectionMatrix, m_viewMatrix;
    glm::vec3 m_position, m_rotation;
};

#endif // CAMERA3D_HPP
