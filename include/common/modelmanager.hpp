#ifndef MODELMANAGER_HPP
#define MODELMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <string>

#include "client/material.hpp"

// vertCoord, texCoord [, normCoord]
struct Vertex3D
{
    glm::vec3 vertCoord;
    glm::vec2 texCoord;
    glm::vec3 normCoord;
};
struct Triangle3D
{
    Vertex3D verts[3];
};

struct Mesh3D
{
    Mesh3D() = default;
    Mesh3D(const std::vector<Triangle3D> &t)
        : tris(t)
    { }
    std::vector<Triangle3D> tris;
};

struct Model3D
{
    Model3D()
        : pMeshes(nullptr), meshesCount(0),
          pMaterials(nullptr), materialsCount(0),
          pMat(nullptr),
          pUserData(nullptr)
    {
    }

    ~Model3D()
    {
        if(meshesCount > 0)
        {
            delete[] pMeshes;
            pMeshes = nullptr;
            meshesCount = 0;
            if(pMaterials)
                delete[] pMaterials;
            pMaterials = nullptr;
        }
    }

    Mesh3D *pMeshes;
    size_t meshesCount;

    Material **pMaterials;
    size_t materialsCount;

    Material* pMat;

    const void *pUserData;
};

class ModelManager
{
public:
    ModelManager();
    ~ModelManager();

    const Model3D *getModel(const std::string &name) const;

    // temp
    void setModelMaterial(const std::string& name, Material* mat);

    void addModel(Model3D *mdl, const std::string &name);
    void loadModel(const void *data, size_t size, const std:: string &name, const char *fmt="glb");
private:
    std::unordered_map<std::string, Model3D*> m_models;
};

#endif // MODELMANAGER_HPP
