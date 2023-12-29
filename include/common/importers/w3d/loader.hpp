#ifndef W3D_LOADER_HPP
#define W3D_LOADER_HPP

#include <assimp/IOStream.hpp>
#include <vector>

#include "common/importers/w3d/struct.hpp"

class W3DFile
{
public:
    W3DFile() = default;
    ~W3DFile() = default;

    void setHierarchy(const W3DHierarchy& hierarchy);
    const W3DHierarchy& getHierarchy() const;

    void addMesh(const W3DMesh& mesh);
    size_t meshCount() const;
    const W3DMesh& mesh(size_t index) const;

    void addAnimation(const W3DAnimation& animation);
    size_t animationCount() const;
    const W3DAnimation& animation(size_t index) const;
private:
    W3DHierarchy m_hierarchy;
    std::vector<W3DMesh> m_meshes;
    std::vector<W3DAnimation> m_animations;
};

class W3DLoader
{
public:
    W3DLoader() = default;
    ~W3DLoader() = default;

    static W3DFile Load(Assimp::IOStream *stream);
};

#endif