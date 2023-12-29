#ifndef MODEL_PRIMITIVE_HPP
#define MODEL_PRIMITIVE_HPP

#include <memory>

#include "common/3d/meshprimitive.hpp"
#include "common/3d/animationprimitive.hpp"

class ModelPrimitive
{
public:
    ModelPrimitive() = default;
    ~ModelPrimitive() = default;

    void addMesh(std::shared_ptr<MeshPrimitive> mesh);
    void addAnimation(std::shared_ptr<AnimationPrimitive> anim);

    size_t meshCount() const;
    const std::string& meshName(size_t index) const;

    std::shared_ptr<MeshPrimitive> mesh(size_t id) const;
    std::shared_ptr<MeshPrimitive> mesh(const std::string& name) const;

    size_t animationCount() const;
    const std::string& animationName(size_t index) const;

    std::shared_ptr<AnimationPrimitive> animation(size_t index) const;
    std::shared_ptr<AnimationPrimitive> animation(const std::string& name) const;
private:
    std::vector<std::shared_ptr<MeshPrimitive>> m_meshes;
    std::vector<std::shared_ptr<AnimationPrimitive>> m_animations;
};

#endif