#ifndef MODELMANAGER_HPP
#define MODELMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include <string>

#include "material.hpp"

// vertCoord, texCoord [, normCoord]
struct Triangle3D
{
    glm::vec3 v0; glm::vec2 t0; glm::vec3 n0;
    glm::vec3 v1; glm::vec2 t1; glm::vec3 n1;
    glm::vec3 v2; glm::vec2 t2; glm::vec3 n2;
};

struct Mesh3D
{
    Mesh3D() {}
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
          pUserData(nullptr)
    {
    }

    ~Model3D()
    {
        if(meshesCount > 0)
        {
            delete[] pMeshes;
            meshesCount = 0;
        }
    }

    Mesh3D *pMeshes;
    size_t meshesCount;

    Material *pMaterials;
    size_t materialsCount;

    const void *pUserData;
};

class ModelManager
{
public:
    ModelManager();
    ~ModelManager();

    const Model3D *getModel(const std::string &name) const;

    void addModel(const Model3D *mdl, const std::string &name);
    void loadModel(const std::string &path, const std::string &name);
private:
    std::unordered_map<std::string, const Model3D*> m_models;
};

#endif // MODELMANAGER_HPP
