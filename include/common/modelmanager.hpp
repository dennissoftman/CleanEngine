#ifndef MODELMANAGER_HPP
#define MODELMANAGER_HPP

#include <unordered_map>
#include <string>

#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/Importer.hpp>

#include "common/loaders/basicresourceloader.hpp"

#include "common/3d/modelprimitive.hpp"


class ResourcesIOStream : public Assimp::IOStream
{
public:
    ResourcesIOStream(const std::string& path);
    ~ResourcesIOStream() = default;

    size_t Read(void* pvBuffer, size_t pSize, size_t pCount) override;
    size_t Write(const void* pvBuffer, size_t pSize, size_t pCount) override;
    aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override;
    size_t Tell() const override;
    size_t FileSize() const override;
    void Flush() override;

private:
    std::shared_ptr<DataResource> m_resource;
    size_t m_offset;
};

class ResourcesIOSystem : public Assimp::IOSystem
{
public:
    ResourcesIOSystem() = default;
    ~ResourcesIOSystem() = default;

    bool Exists(const char* pFile) const override;
    char getOsSeparator() const override;
    Assimp::IOStream* Open(const char* pFile, const char* pMode = "rb") override;
    void Close(Assimp::IOStream* pFile) override;
};

class ModelManager
{
public:
    ModelManager();
    ~ModelManager();

    void import_model(const std::string& path, const std::string& name, bool allocateGraphics=true);

    size_t getModelId(const std::string& path) const;
    std::shared_ptr<AnimationPrimitive> getAnimation(const std::string& modelName, const std::string& animationName) const;
private:
    void allocate_graphics(const std::string& name, std::shared_ptr<ModelPrimitive> model);

    Assimp::Importer m_importer;
    std::unordered_map<std::string, std::shared_ptr<ModelPrimitive>> m_models;
    std::unordered_map<std::string, size_t> m_modelIDs;  // imported model ids
};

#endif // MODELMANAGER_HPP
