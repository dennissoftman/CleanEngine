#ifndef STATICMESH_HPP
#define STATICMESH_HPP

#include "common/entities/component.hpp"
#include "common/modelmanager.hpp"

class StaticMesh : public Component
{
public:
    StaticMesh(std::shared_ptr<Entity> parent);
    ~StaticMesh() override;

    void draw(Renderer *rend) override;
    void update(double dt) override;

    bool isDrawable() const override;
    const char *getName() const override;

    void setModel(const Model3D *mdl);

    void updateMatrix();
protected:
    std::string m_name;
    const Model3D *rObj;
    glm::mat4 m_modelMatrix;
};

#endif // STATICMESH_HPP
