#ifndef W3D_IMPORTER_HPP
#define W3D_IMPORTER_HPP

#include <assimp/BaseImporter.h>
#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>

class W3DImporter : public Assimp::BaseImporter
{
public:
    W3DImporter() = default;
    ~W3DImporter() override = default;

    bool CanRead(const std::string& pFile, Assimp::IOSystem* pIOHandler, bool checkSig) const override;
    void SetupProperties(const Assimp::Importer *pImp) override;
    const aiImporterDesc *GetInfo() const override;
    void InternReadFile(const std::string &pFile, aiScene *pScene, Assimp::IOSystem *pIOHandler) override;
};

#endif