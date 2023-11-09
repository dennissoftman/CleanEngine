#ifndef CAMERA3D_HPP
#define CAMERA3D_HPP

#include "common/entities/entity.hpp"
#include "common/dirty_flag.hpp"
#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>

#define CAMERA_UP (glm::vec3(0, 1, 0))

class Camera3D : public Entity
{
public:
    Camera3D(float fov=90.f, float aspect=16.f/9.f, float znear=0.1f, float zfar=1000.f);

    void setVisible(bool yes) override;

    Camera3D &operator =(const Camera3D &other);

    void setPosition(const glm::vec3 &pos) override;
    void setRotation(const glm::quat &qrot) override;
    void setEulerRotation(const glm::vec3 &rot) override;
    void setScale(const glm::vec3 &scale) override; // camera probably won't be ever scaled

    const glm::vec3 &frontVector();
    const glm::vec3 &rightVector();
    const glm::vec3 &upVector();

    void setFOV(float fov);
    void setAspect(float aspect);
    void setClipping(float near, float far);

    const glm::mat4 &getViewMatrix();
    const glm::mat4 &getProjectionMatrix();
protected:
    void updateMatrices();

    float m_fov, m_aspect, m_znear, m_zfar;
    clean::dirty_flag<glm::mat4> m_projectionMatrix, m_viewMatrix;

    glm::vec3 m_front, m_right, m_up;
};

#endif // CAMERA3D_HPP
