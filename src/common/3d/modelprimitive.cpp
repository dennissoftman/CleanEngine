#include <stdexcept>
#include <fmt/format.h>

#include "common/3d/modelprimitive.hpp"


void ModelPrimitive::addMesh(std::shared_ptr<MeshPrimitive> mesh)
{
    m_meshes.push_back(mesh);
}

void ModelPrimitive::addAnimation(std::shared_ptr<AnimationPrimitive> anim)
{
    m_animations.push_back(anim);
}

size_t ModelPrimitive::meshCount() const
{
    return m_meshes.size();
}

const std::string& ModelPrimitive::meshName(size_t index) const
{
    if (index >= m_meshes.size())
        throw std::runtime_error(fmt::format("Model doesn't contain meshID {}", index));
    return m_meshes.at(index)->name();
}

std::shared_ptr<MeshPrimitive> ModelPrimitive::mesh(size_t index) const
{
    if (index >= m_meshes.size())
        throw std::runtime_error(fmt::format("Model doesn't contain meshID {}", index));
    return m_meshes.at(index);
}

std::shared_ptr<MeshPrimitive> ModelPrimitive::mesh(const std::string& name) const
{
    for (auto mesh : m_meshes)
    {
        if (mesh->name() == name)
            return mesh;
    }
    throw std::runtime_error(fmt::format("Model doesn't contain mesh '{}'", name));
}

size_t ModelPrimitive::animationCount() const
{
    return m_animations.size();
}

const std::string& ModelPrimitive::animationName(size_t index) const
{
    if (index >= m_animations.size())
        throw std::runtime_error(fmt::format("Model doesn't contain animID {}", index));
    return m_animations.at(index)->name();
}

std::shared_ptr<AnimationPrimitive> ModelPrimitive::animation(size_t index) const
{
    if (index >= m_animations.size())
        throw std::runtime_error(fmt::format("Model doesn't contain animID {}", index));
    return m_animations.at(index);
}

std::shared_ptr<AnimationPrimitive> ModelPrimitive::animation(const std::string& name) const
{
    for (auto anim : m_animations)
    {
        if (anim->name() == name)
            return anim;
    }
    throw std::runtime_error(fmt::format("Model doesn't contain animation '{}'", name));
}

