#ifndef CAMERA3D_HPP
#define CAMERA3D_HPP

#include "common/entities/entity.hpp"
#include "client/renderer.hpp"

#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>

#define CAMERA_UP (glm::vec3(0, 1, 0))

#include "common/dirty_flag.hpp"
#include <boost/signals2.hpp>

class Camera3D : public Entity
{
public:
    Camera3D(float fov=90.f, float aspect=16.f/9.f, float znear=0.1f, float zfar=1000.f);

    void draw(Renderer *rend) override;
    void update(double dt) override;

    // camera visible? What should it mean?
    void setVisible(bool yes) override;

    // you probably won't use it
    void destroy() override;

    Camera3D &operator =(const Camera3D &other);

    void setPosition(const glm::vec3 &pos) override;
    void setRotation(const glm::quat &qrot) override;
    void setEulerRotation(const glm::vec3 &rot) override;
    void setScale(const glm::vec3 &scale) override; // camera probably won't be ever scaled

    const glm::vec3 &getPosition() const override;
    glm::vec3 getEulerRotation() const override;
    const glm::quat &getRotation() const override;
    const glm::vec3 &getScale() const override;

    void setScene(Scene3D *parent) override;
    Scene3D *getParentScene() const override;

    const char *getType() override;

    const glm::vec3 &frontVector();
    const glm::vec3 &rightVector();
    const glm::vec3 &upVector();

    void setFOV(float fov);
    void setAspect(float aspect);
    void setClipping(float near, float far);

    const glm::mat4 &getViewMatrix();
    const glm::mat4 &getProjectionMatrix();

    // events
    void updateSubscribe(const std::function<void (Entity*, double)> &callb) override;
    void destroySubscribe(const std::function<void (Entity*)> &callb) override;
private:
    void updateMatrices();

    boost::signals2::signal<void(Entity*, double)> m_updateEvents;
    boost::signals2::signal<void(Entity*)> m_destroyEvents;

    Scene3D *m_parentScene;

    float m_fov, m_aspect, m_znear, m_zfar;
    clean::dirty_flag<glm::mat4> m_projectionMatrix, m_viewMatrix;
    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scale;

    glm::vec3 m_front, m_right, m_up;
};

#endif // CAMERA3D_HPP
