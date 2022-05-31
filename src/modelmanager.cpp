#include "modelmanager.hpp"
#include <stdexcept>
#include <cassert>

#include "servicelocator.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

static const char *MODULE_NAME = "ModelManager";

ModelManager::ModelManager()
{

}

ModelManager::~ModelManager()
{
    for(auto &kv : m_models)
    {
        delete kv.second;
    }
}

const Model3D *ModelManager::getModel(const std::string &name) const
{
    if(m_models.find(name) == m_models.end())
        return nullptr;
    if(m_models.at(name) == nullptr)
        throw std::runtime_error("Unexpected error while getting model");
    return m_models.at(name);
}

void ModelManager::setModelMaterial(const std::string& name, Material* mat)
{
    if (m_models.find(name) == m_models.end())
        throw std::runtime_error("Model not found");
    if(mat != NULL)
        m_models[name]->pMat = mat;
}

void ModelManager::addModel(Model3D *mdl, const std::string &name)
{
    if(mdl)
        m_models[name] = mdl;
}

void ModelManager::loadModel(const void *data, size_t size, const std::string &name, const char *fmt)
{
    Model3D *outModel = nullptr;
    Assimp::Importer loader;
    {
        const aiScene *scn = loader.ReadFileFromMemory(data, size, aiProcess_Triangulate | aiProcess_EmbedTextures, fmt);

        if(scn == nullptr)
        {
            ServiceLocator::getLogger().error(MODULE_NAME, "Assimp error: " + std::string(loader.GetErrorString()));
            return;
        }

        if(scn->mNumMeshes == 0)
        {
            ServiceLocator::getLogger().error(MODULE_NAME, "Cannot load model without meshes!");
            return;
        }

        outModel = new Model3D();
        outModel->meshesCount = scn->mNumMeshes;
        outModel->pMeshes = new Mesh3D[outModel->meshesCount];
        outModel->materialsCount = scn->mNumMaterials;
        outModel->pMaterials = new Material*[outModel->materialsCount];

        for(size_t i=0; i < scn->mNumMaterials; i++)
        {
            Material *mat = nullptr;
            // set color/image/properties
            aiMaterial *aiMat = scn->mMaterials[i];
            aiString matName;
            aiMat->Get(AI_MATKEY_NAME, matName);
            if((mat = ServiceLocator::getMatManager().getMaterial(matName.C_Str())) == nullptr)
            {
                mat = Material::create();
                aiColor4D color;
                aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
                mat->setColor(glm::vec4(color.r, color.g, color.b, color.a), "color");
                mat->init();
            }
            //
            outModel->pMaterials[i] = mat;
        }

        for(size_t i=0; i < scn->mNumMeshes; i++) // meshes
        {
            aiMesh *mesh = scn->mMeshes[i];
            for(size_t j=0; j < mesh->mNumFaces; j++) // triangles
            {
                aiFace face = mesh->mFaces[j];
                assert(face.mNumIndices == 3 && "Mesh must be triangulated"); // triangle

                aiVector3D v0 = mesh->mVertices[face.mIndices[0]],
                           v1 = mesh->mVertices[face.mIndices[1]],
                           v2 = mesh->mVertices[face.mIndices[2]];

                aiVector3D t0{}, t1{}, t2{};
                if(mesh->mTextureCoords[0])
                {
                    t0 = mesh->mTextureCoords[0][face.mIndices[0]];
                    t1 = mesh->mTextureCoords[0][face.mIndices[1]];
                    t2 = mesh->mTextureCoords[0][face.mIndices[2]];
                }

                aiVector3D n0{1, 0, 0}, n1{0, 1, 0}, n2{0, 0, 1};
                if(mesh->mNormals)
                {
                    n0 = mesh->mNormals[face.mIndices[0]];
                    n1 = mesh->mNormals[face.mIndices[1]];
                    n2 = mesh->mNormals[face.mIndices[2]];
                }

                {
                    aiMaterial *mat = scn->mMaterials[mesh->mMaterialIndex];
                }

                outModel->pMeshes[i].tris.push_back(
                            Triangle3D{
                                Vertex3D{glm::vec3(v0.x, v0.y, v0.z), glm::vec2(t0.x, t0.y), glm::vec3(n0.x, n0.y, n0.z)},
                                Vertex3D{glm::vec3(v1.x, v1.y, v1.z), glm::vec2(t1.x, t1.y), glm::vec3(n1.x, n1.y, n1.z)},
                                Vertex3D{glm::vec3(v2.x, v2.y, v2.z), glm::vec2(t2.x, t2.y), glm::vec3(n2.x, n2.y, n2.z)}
                            });
            }
        }
    }

    if(outModel)
        addModel(outModel, name);
}
