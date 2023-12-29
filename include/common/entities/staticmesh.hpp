#ifndef STATICMESH_HPP
#define STATICMESH_HPP

#include <glm/glm.hpp>
#include <string>

#include "common/entities/meshcomponent.hpp"

class StaticMesh : public MeshComponent
{
public:
    StaticMesh(std::shared_ptr<Entity> parent);
    ~StaticMesh() override;

    void draw(Renderer *rend) override;
    void update(double dt) override;

    void setModelId(uint32_t id) override;

    void show() override;
    void hide() override;
    bool isDrawable() const override;
    // StaticMesh
    void castShadow(bool cast) override;
    bool isCastingShadow() const override;

    void setTranslucent(bool translucent) override;
    bool isTranslucent() const override;

    const char *getName() const override;

    void updateMatrix();

    static std::shared_ptr<StaticMesh> createComponent(std::shared_ptr<Entity> parent);
protected:
    std::string m_name;
    glm::mat4 m_modelMatrix;
    uint32_t m_meshId;
    bool m_visible;
    bool m_castShadow;
    bool m_translucent;
};

#endif // STATICMESH_HPP
