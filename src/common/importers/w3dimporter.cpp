#include <assimp/IOSystem.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/importerdesc.h>

#include <glm/gtx/euler_angles.hpp>

#include <map>

#include <spdlog/spdlog.h>

#include "common/importers/w3dimporter.hpp"
#include "common/importers/w3d/loader.hpp"


bool W3DImporter::CanRead(const std::string& pFile, Assimp::IOSystem* pIOHandler, bool checkSig) const
{
    return pIOHandler->Exists(pFile);
}

void W3DImporter::SetupProperties(const Assimp::Importer *pImp)
{

}

const aiImporterDesc *W3DImporter::GetInfo() const
{
    aiImporterDesc *desc = new aiImporterDesc{};
    desc->mName = "W3D Importer";
    desc->mAuthor = "Dennis Softman";
    desc->mFileExtensions = "w3d";
    desc->mFlags = aiImporterFlags_SupportBinaryFlavour;
    return desc;
}

void W3DImporter::InternReadFile(const std::string &pFile, aiScene *pScene, Assimp::IOSystem *pIOHandler)
{
    auto fstream = pIOHandler->Open(pFile, "rb");

    spdlog::debug("Reading W3D file '{}'", pFile);
    W3DFile w3dfile{};
    try
    {
        w3dfile = W3DLoader::Load(fstream);
    }
    catch(const std::exception &e)
    {
        spdlog::error("Failed to load W3D file: {}", e.what());
        return;
    }

    std::map<std::string, W3DPivot> pivotMap;
    for(uint32_t pivotIdx = 0; pivotIdx < w3dfile.getHierarchy().header().numPivots(); pivotIdx++)
    {
        const auto& pivot = w3dfile.getHierarchy().pivot(pivotIdx);
        pivotMap[pivot.name()] = pivot;
    }

    pScene->mMeshes = new aiMesh*[w3dfile.meshCount()];
    pScene->mNumMeshes = (unsigned int)w3dfile.meshCount();
    for(size_t meshIdx=0; meshIdx < w3dfile.meshCount(); meshIdx++)
    {
        const auto& w3dmesh = w3dfile.mesh(meshIdx);
        aiMesh *mesh = new aiMesh{};
        mesh->mName = aiString{w3dmesh.header().meshName()};

        W3DPivot pivot{};
        if(pivotMap.find(w3dmesh.header().meshName()) == pivotMap.end())
            spdlog::warn("Mesh '{}' has no pivot", w3dmesh.header().meshName());
        else
            pivot = pivotMap[w3dmesh.header().meshName()];

        mesh->mVertices = new aiVector3D[w3dmesh.header().numVertices()];
        mesh->mNormals = new aiVector3D[w3dmesh.header().numVertices()];
        mesh->mNumVertices = w3dmesh.header().numVertices();
        for(uint32_t vi=0; vi < w3dmesh.header().numVertices(); vi++)
        {
            auto vertex = w3dmesh.vertex(vi).vector();
            // transform vertex by pivot
            if(pivot.valid())
            {
                glm::mat4 transform = glm::mat4(1, 0, 0, 0, 0, 0, 1, 0, 0, -1, 0, 0, 0, 0, 0, 1);

                glm::vec3 trs = glm::vec4(pivot.translation().vector(), 1.f) * transform;
                glm::vec3 rot = glm::vec4(pivot.eulerAngles().vector(), 1.f) * transform;

                vertex = glm::vec4(vertex, 1.f) * transform;
                vertex += trs;
                vertex = glm::vec4(vertex, 1.f) * glm::eulerAngleZYX(rot.z, rot.y, rot.x);
            }
            auto normal = w3dmesh.normal(vi).vector();
            mesh->mVertices[vi] = aiVector3D{vertex.x, vertex.y, vertex.z};
            mesh->mNormals[vi] = aiVector3D{normal.x, normal.y, normal.z};
        }

        if(w3dmesh.materialInfo().passCount() == 0)
            spdlog::debug("Mesh '{}' has no material passes", mesh->mName.C_Str());
        else
        {
            // if(w3dmesh.materialInfo().passCount() > 0)
            //     spdlog::debug("Mesh '{}' has {} material passes but only first one will be used", mesh->mName.C_Str(), w3dmesh.materialInfo().passCount());
            
            const auto& materialPass = w3dmesh.materialPass(0);
            const auto& textureStage = materialPass.textureStage(0);
            mesh->mTextureCoords[0] = new aiVector3D[mesh->mNumVertices];
            for(uint32_t vi=0; vi < mesh->mNumVertices; vi++)
            {
                const auto& texCoord = textureStage.texCoord(vi).texCoord();
                mesh->mTextureCoords[0][vi] = aiVector3D{texCoord.x, texCoord.y, 0};
            }
        }

        mesh->mFaces = new aiFace[w3dmesh.header().numTriangles()];
        mesh->mNumFaces = w3dmesh.header().numTriangles();
        for(uint32_t triIdx=0; triIdx < w3dmesh.header().numTriangles(); triIdx++)
        {
            const auto& triangle = w3dmesh.triangle(triIdx);
            aiFace face{};
            face.mIndices = new unsigned int[3];
            face.mNumIndices = 3;
            face.mIndices[0] = triangle.vertexIndex(0);
            face.mIndices[1] = triangle.vertexIndex(1);
            face.mIndices[2] = triangle.vertexIndex(2);
            mesh->mFaces[triIdx] = face;
        }

        pScene->mMeshes[meshIdx] = mesh;
    }

    pIOHandler->Close(fstream);
}