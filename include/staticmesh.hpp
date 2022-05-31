#ifndef STATICMESH_HPP
#define STATICMESH_HPP

#include "scene3d.hpp"
#include "entity.hpp"

class StaticMesh : public Entity
{
public:
    StaticMesh();

    ~StaticMesh() override;

    void draw(Renderer *rend) override;
    void update(double dt) override;

    void setVisible(bool yes) override;
    void destroy() override;

    void setModel(const Model3D *mdl);

    void updateMatrix();

    void setPosition(const glm::vec3 &pos) override;
    void setRotation(const glm::quat &rot) override;
    void setEulerRotation(const glm::vec3 &rot) override;
    void setScale(const glm::vec3 &scl) override;

    const glm::vec3 &getPosition() const override;
    glm::vec3 getEulerRotation() const override;
    const glm::quat &getRotation() const override;
    const glm::vec3 &getScale() const override;

    void setScene(Scene3D *parent) override;
    Scene3D *getParentScene() const override;
protected:
    // TMP
    const Model3D *rObj;
    //
    bool m_visible;
    int m_changedMatrix;
    glm::mat4 modelMatrix;
    glm::vec3 m_pos;
    glm::quat m_rot;
    glm::vec3 m_scale;

    Scene3D *m_parentScene;
};

#endif // STATICMESH_HPP
