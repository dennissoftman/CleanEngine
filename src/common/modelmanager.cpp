#include <stdexcept>
#include <cassert>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <fmt/format.h>
#include <boost/algorithm/string.hpp>
#include <spdlog/spdlog.h>

#include "common/modelmanager.hpp"
#include "common/servicelocator.hpp"
#include "common/importers/w3dimporter.hpp"

#include "common/3d/animationprimitive.hpp"


ModelManager::ModelManager()
{
    m_importer.SetIOHandler(new ResourcesIOSystem());
    m_importer.RegisterLoader(new W3DImporter());
}

ModelManager::~ModelManager()
{

}

void enumerateNodeMeshes(aiNode* node, std::vector<uint32_t>& meshes)
{
    for(unsigned int i=0; i < node->mNumMeshes; i++)
    {
        meshes.push_back(node->mMeshes[i]);
    }

    for(unsigned int i=0; i < node->mNumChildren; i++)
    {
        enumerateNodeMeshes(node->mChildren[i], meshes);
    }
}

void unwrapNodeTree(aiNode* node, std::unordered_map<std::string, aiNode*>& unwrappedNodeTree)
{
    unwrappedNodeTree.try_emplace(node->mName.C_Str(), node);
    for(unsigned int i=0; i < node->mNumChildren; i++)
    {
        unwrapNodeTree(node->mChildren[i], unwrappedNodeTree);
    }
}

void cumulativeNodeTransform(aiNode* node, aiMatrix4x4& transform)
{
    transform = node->mTransformation * transform;
    if (node->mParent)
        cumulativeNodeTransform(node->mParent, transform);
}

void ModelManager::import_model(const std::string &path, const std::string &name, bool allocateGraphics)
{
    const aiScene* scene = m_importer.ReadFile(path, aiProcess_Triangulate |
                                                     aiProcess_GenNormals |
                                                     aiProcess_JoinIdenticalVertices);
    if(scene == nullptr) {
        throw std::runtime_error(fmt::format("Failed to import model '{}': {}", name, m_importer.GetErrorString()));
    }

    if(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        throw std::runtime_error(fmt::format("Model data '{}' is malformed: ", name));
    }

    if(!scene->HasMeshes())
    {
        throw std::runtime_error(fmt::format("Model data '{}' has no meshes", name));
    }

    std::unordered_map<std::string, aiNode*> unwrappedNodeTree;
    unwrapNodeTree(scene->mRootNode, unwrappedNodeTree);

    auto model = std::make_shared<ModelPrimitive>();
    for(unsigned int meshId = 0; meshId < scene->mNumMeshes; meshId++)
    {
        const aiMesh* mesh = scene->mMeshes[meshId];
        aiString meshName = mesh->mName;
        if(!mesh->HasPositions())
        {
            throw std::runtime_error(fmt::format("Model data '{}' has no vertices", name));
        }
        if(!mesh->HasNormals())
        {
            spdlog::warn("Model data '{}' has no normals", name);
        }
        if(!mesh->HasTextureCoords(0))
        {
            spdlog::warn("Model data '{}' has no texture coordinates", name);
            continue; // skip for now
        }
        if(!mesh->HasFaces())
        {
            throw std::runtime_error(fmt::format("Model data '{}' has no faces", name));
        }

        auto meshPrimitive = std::make_shared<MeshPrimitive>(meshName.C_Str(), meshId);
        for(unsigned int j = 0; j < mesh->mNumVertices; j++)
        {
            aiVector3D pos = mesh->mVertices[j];
            aiVector3D norm = norm = mesh->mNormals[j];
            const aiVector3D& tex = mesh->mTextureCoords[0][j];

            auto meshNodeIt = unwrappedNodeTree.find(mesh->mName.C_Str());
            if(meshNodeIt != unwrappedNodeTree.end())
            {
                aiMatrix4x4 transform = meshNodeIt->second->mTransformation;
                pos = transform * pos;
                norm = transform * norm;
            }

            meshPrimitive->addVertex(VertexPrimitive(glm::vec3(pos.x, pos.y, pos.z),
                                                    glm::vec3(norm.x, norm.y, norm.z),
                                                    glm::vec2(tex.x, tex.y)));
        }

        for(unsigned int j = 0; j < mesh->mNumFaces; j++)
        {
            aiFace face = mesh->mFaces[j];
            if(face.mNumIndices != 3)
            {
                throw std::runtime_error(fmt::format("Model data '{}' has non-triangular faces", name));
            }
            meshPrimitive->addIndex(face.mIndices[0]);
            meshPrimitive->addIndex(face.mIndices[1]);
            meshPrimitive->addIndex(face.mIndices[2]);
        }

        model->addMesh(meshPrimitive);
    }

    for(unsigned int i=0; i < scene->mNumMaterials; i++)
    {
        const aiMaterial* material = scene->mMaterials[i];
        aiString materialName;
        material->Get(AI_MATKEY_NAME, materialName);
        spdlog::info("Texture name: {}", materialName.C_Str());
    }

    for (unsigned int i = 0; i < scene->mNumAnimations; i++)
    {
        const aiAnimation* animation = scene->mAnimations[i];
        aiString animName = animation->mName;

        auto animationPrimitive = std::make_shared<AnimationPrimitive>(animName.C_Str(), animation->mDuration);
        for (unsigned int channelId = 0; channelId < animation->mNumChannels; channelId++)
        {
            const aiNodeAnim* nodeAnim = animation->mChannels[channelId];
            aiString nodeName = nodeAnim->mNodeName;
            auto nodeIt = unwrappedNodeTree.find(nodeAnim->mNodeName.C_Str());
            if (nodeIt == unwrappedNodeTree.end())
            {
                spdlog::warn("No suitable node named '{}' found", nodeName.C_Str());
                continue;
            }

            aiNode* meshNode = nodeIt->second;
            std::vector<unsigned int> channelMeshIds;
            enumerateNodeMeshes(meshNode, channelMeshIds);
            animationPrimitive->setMeshIds(channelId, channelMeshIds);
            // aiMatrix4x4 nodeTransform; // identity matrix
            // cumulativeNodeTransform(meshNode, nodeTransform);

            uint32_t frameCount = std::max(nodeAnim->mNumPositionKeys, std::max(nodeAnim->mNumRotationKeys, nodeAnim->mNumScalingKeys));
            for (unsigned int frameId = 0; frameId < frameCount; frameId++)
            {
                AnimationKeyFrame keyframe;
                double timecode = 0.0;
                if (frameId < nodeAnim->mNumPositionKeys)
                {
                    aiVectorKey posKey = nodeAnim->mPositionKeys[frameId];
                    timecode = posKey.mTime;
                    aiVector3D pos = posKey.mValue;
                    keyframe.setPosition(glm::vec3(pos.x, pos.y, pos.z));
                }
                if (frameId < nodeAnim->mNumRotationKeys)
                {
                    aiQuatKey rotKey = nodeAnim->mRotationKeys[frameId];
                    timecode = std::max(timecode, rotKey.mTime);
                    aiQuaternion rot = rotKey.mValue;
                    keyframe.setRotation(glm::quat(rot.w, rot.x, rot.y, rot.z));
                }
                if (frameId < nodeAnim->mNumScalingKeys)
                {
                    aiVectorKey sclKey = nodeAnim->mScalingKeys[frameId];
                    timecode = std::max(timecode, sclKey.mTime);
                    aiVector3D scl = sclKey.mValue;
                    keyframe.setScale(glm::vec3(scl.x, scl.y, scl.z));
                }
                keyframe.setTime(timecode);

                animationPrimitive->addKeyframe(channelId, keyframe);
            }
        }
        model->addAnimation(animationPrimitive);
    }

    auto r = m_models.try_emplace(name, model);
    if(!r.second)
    {
        throw std::runtime_error(fmt::format("Model '{}' already exists", name));
    }

    m_importer.FreeScene();

    if(allocateGraphics)
    {
        allocate_graphics(name, model);
    }
}

void ModelManager::allocate_graphics(const std::string& name, std::shared_ptr<ModelPrimitive> model)
{
    m_modelIDs.try_emplace(name, ServiceLocator::getRenderer().allocateModel(model));
}

size_t ModelManager::getModelId(const std::string &name) const
{
    auto it = m_modelIDs.find(name);
    if(it == m_modelIDs.end())
    {
        throw std::runtime_error(fmt::format("Model '{}' does not exist", name));
    }
    return it->second;
}

std::shared_ptr<AnimationPrimitive> ModelManager::getAnimation(const std::string& modelName, const std::string& animationName) const
{
    auto modelIt = m_models.find(modelName);
    if (modelIt == m_models.end())
        throw std::runtime_error(fmt::format("Model '{}' not found", modelName));

    return modelIt->second->animation(animationName);
}

ResourcesIOStream::ResourcesIOStream(const std::string &path)
    : m_offset(0), m_resource(ServiceLocator::getResourceManager().get(path))
{
}

size_t ResourcesIOStream::Read(void *pvBuffer, size_t pSize, size_t pCount)
{
    size_t size = pSize * pCount;
    if((m_offset + size) > m_resource->data.size())
    {
        size = m_resource->data.size() - m_offset;
    }
    memcpy(pvBuffer, m_resource->data.data() + m_offset, size);
    m_offset += size;
    return size;
}

size_t ResourcesIOStream::Write(const void *pvBuffer, size_t pSize, size_t pCount)
{
    (void)pvBuffer;
    (void)pSize;
    (void)pCount;
    return 0;
}

aiReturn ResourcesIOStream::Seek(size_t pOffset, aiOrigin pOrigin)
{
    switch(pOrigin)
    {
    case aiOrigin_SET:
        m_offset = pOffset;
        break;
    case aiOrigin_CUR:
        m_offset += pOffset;
        break;
    case aiOrigin_END:
        m_offset = m_resource->data.size() - pOffset;
        break;
    default:
        return aiReturn_FAILURE;
    }
    return aiReturn_SUCCESS;
}

size_t ResourcesIOStream::Tell() const
{
    return m_offset;
}

size_t ResourcesIOStream::FileSize() const
{
    return m_resource->data.size();
}

void ResourcesIOStream::Flush()
{
}

Assimp::IOStream* ResourcesIOSystem::Open(const char *pFile, const char *pMode)
{
    (void)pMode;
    return new ResourcesIOStream(pFile);
}

void ResourcesIOSystem::Close(Assimp::IOStream *pFile)
{
    delete pFile;
}

bool ResourcesIOSystem::Exists(const char *pFile) const
{
    return ServiceLocator::getResourceManager().contains(pFile);
}

char ResourcesIOSystem::getOsSeparator() const
{
    return '/';
}