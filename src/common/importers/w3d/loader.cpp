#include <boost/endian.hpp>
#include <spdlog/spdlog.h>

#include "common/importers/w3d/loader.hpp"
#include "common/importers/w3d/enum.hpp"

static const char* chunk_type_to_str(W3D_CHUNK type)
{
    switch (type)
    {
        case W3D_CHUNK::eMESH:
            return "CHUNK_MESH";
        case W3D_CHUNK::eHIERARCHY:
            return "CHUNK_HIERARCHY";
        case W3D_CHUNK::eANIMATION:
            return "CHUNK_ANIMATION";
        case W3D_CHUNK::eCOMPRESSED_ANIMATION:
            return "CHUNK_COMPRESSED_ANIMATION";
        case W3D_CHUNK::eMORPH_ANIMATION:
            return "CHUNK_MORPH_ANIMATION";
        case W3D_CHUNK::eHMODEL:
            return "CHUNK_HMODEL";
        case W3D_CHUNK::eLODMODEL:
            return "CHUNK_LODMODEL";
        case W3D_CHUNK::eCOLLECTION:
            return "CHUNK_COLLECTION";
        case W3D_CHUNK::ePOINTS:
            return "CHUNK_POINTS";
        case W3D_CHUNK::eLIGHT:
            return "CHUNK_LIGHT";
        case W3D_CHUNK::eEMITTER:
            return "CHUNK_EMITTER";
        case W3D_CHUNK::eAGGREGATE:
            return "CHUNK_AGGREGATE";
        case W3D_CHUNK::eHLOD:
            return "CHUNK_HLOD";
        case W3D_CHUNK::eBOX:
            return "CHUNK_BOX";
        case W3D_CHUNK::eSPHERE:
            return "CHUNK_SPHERE";
        case W3D_CHUNK::eRING:
            return "CHUNK_RING";
        case W3D_CHUNK::eNULL_OBJECT:
            return "CHUNK_NULL_OBJECT";
        case W3D_CHUNK::eLIGHTSCAPE:
            return "CHUNK_LIGHTSCAPE";
        case W3D_CHUNK::eDAZZLE:
            return "CHUNK_DAZZLE";
        case W3D_CHUNK::eSOUNDROBJ:
            return "CHUNK_SOUNDROBJ";
    }    
    return "CHUNK_UNKNOWN";
}

static W3DHierarchy ReadHierarchy(Assimp::IOStream *stream, uint32_t chunkSize)
{
    W3DHierarchy hierarchy{};
    // TODO: implement animation hierarchy loading

    size_t startOffset = stream->Tell();
    while(stream->Tell() < (startOffset + chunkSize))
    {
        W3DChunkHeader chunk(stream);
        switch(chunk.type())
        {
            case W3D_CHUNK::eHIERARCHY_HEADER:
                hierarchy.setHeader(W3DHierarchyHeader(stream));
                break;
            case W3D_CHUNK::ePIVOTS:
                for(uint32_t i = 0; i < hierarchy.header().numPivots(); i++)
                {
                    hierarchy.addPivot(W3DPivot(stream));
                }
                break;
            default:
                stream->Seek(chunk.size(), aiOrigin_CUR);
                break;
        }
    }
    return hierarchy;
}

static W3DMaterial ReadVertexMaterial(Assimp::IOStream *stream, uint32_t chunkSize)
{
    W3DMaterial material{};

    size_t startOffset = stream->Tell();
    while(stream->Tell() < (startOffset + chunkSize))
    {
        W3DChunkHeader chunk(stream);
        switch(chunk.type())
        {
            case W3D_CHUNK::eVERTEX_MATERIAL_NAME:
            {
                std::string name(chunk.size(), '\0');
                stream->Read(name.data(), 1, chunk.size());
                name.pop_back(); // remove null terminator
                material.setName(name);
                break;
            }
            case W3D_CHUNK::eVERTEX_MAPPER_ARGS0:
            {
                std::string args(chunk.size(), '\0');
                stream->Read(args.data(), 1, chunk.size());
                args.pop_back(); // remove null terminator
                material.setMapperArgs0(args);
                break;
            }
            case W3D_CHUNK::eVERTEX_MAPPER_ARGS1:
            {
                std::string args(chunk.size(), '\0');
                stream->Read(args.data(), 1, chunk.size());
                args.pop_back(); // remove null terminator
                material.setMapperArgs1(args);
                break;
            }
            case W3D_CHUNK::eVERTEX_MATERIAL_INFO:
            {
                material.setVertexMaterialInfo(W3DVertexMaterial(stream));
                break;
            }
            default:
            {
                stream->Seek(chunk.size(), aiOrigin_CUR);
                break;
            }
        }
    }
    return material;
}

static W3DTexture ReadTexture(Assimp::IOStream *stream, uint32_t chunkSize)
{
    W3DTexture texture{};

    size_t startOffset = stream->Tell();
    while(stream->Tell() < (startOffset + chunkSize))
    {
        W3DChunkHeader chunk(stream);
        switch(chunk.type())
        {
            case W3D_CHUNK::eTEXTURE_NAME:
            {
                std::string name(chunk.size(), '\0');
                stream->Read(name.data(), 1, chunk.size());
                name.pop_back(); // remove null terminator
                texture.setName(name);
                break;
            }
            case W3D_CHUNK::eTEXTURE_INFO:
            {
                texture.setTextureInfo(W3DTextureInfo(stream));
                break;
            }
            default:
            {
                stream->Seek(chunk.size(), aiOrigin_CUR);
                break;
            }
        }
    }
    return texture;
}

static W3DTextureStage ReadTextureStage(Assimp::IOStream *stream, uint32_t chunkSize)
{
    W3DTextureStage textureStage{};

    size_t startOffset = stream->Tell();
    while(stream->Tell() < (startOffset + chunkSize))
    {
        W3DChunkHeader chunk(stream);
        switch(chunk.type())
        {
            case W3D_CHUNK::eTEXTURE_IDS:
            {
                for(size_t i=0; i < chunk.size(); i += sizeof(uint32_t))
                {
                    uint32_t tmp;
                    stream->Read(&tmp, sizeof(uint32_t), 1);
                    textureStage.addTextureID(boost::endian::little_to_native(tmp));
                }
                break;
            }
            case W3D_CHUNK::ePER_FACE_TEXCOORD_IDS:
            {
                for(size_t i=0; i < chunk.size(); i += 3*sizeof(int32_t))
                {
                    textureStage.addPerFaceTexCoordID(W3DVectori(stream));
                }
                break;
            }
            case W3D_CHUNK::eSTAGE_TEXCOORDS:
            {
                for(size_t i=0; i < chunk.size(); i += 2*sizeof(float))
                {
                    textureStage.addTexCoord(W3DTexCoord(stream));
                }
                break;
            }
            default:
            {
                stream->Seek(chunk.size(), aiOrigin_CUR);
                break;
            }
        }
    }

    return textureStage;
}

static W3DMaterialPass ReadMaterialPass(Assimp::IOStream *stream, uint32_t chunkSize)
{
    W3DMaterialPass materialPass{};

    size_t startOffset = stream->Tell();
    while(stream->Tell() < (startOffset + chunkSize))
    {
        W3DChunkHeader chunk(stream);
        switch (chunk.type())
        {
            case W3D_CHUNK::eVERTEX_MATERIAL_IDS:
            {
                uint32_t tmp;
                for(size_t i=0; i < chunk.size(); i += sizeof(uint32_t))
                {
                    stream->Read(&tmp, sizeof(uint32_t), 1);
                    materialPass.addVertexMaterialID(boost::endian::little_to_native(tmp));
                }
                break;
            }
            case W3D_CHUNK::eSHADER_IDS:
            {
                uint32_t tmp;
                for(size_t i=0; i < chunk.size(); i += sizeof(uint32_t))
                {
                    stream->Read(&tmp, sizeof(uint32_t), 1);
                    materialPass.addShaderID(boost::endian::little_to_native(tmp));
                }
                break;
            }
            case W3D_CHUNK::eDCG:
            {
                for(size_t i=0; i < chunk.size(); i += 4*sizeof(float)) // 4 floats per DCG
                {
                    materialPass.addDCG(W3DRGBA(stream));
                }
                break;
            }
            case W3D_CHUNK::eDIG:
            {
                for(size_t i=0; i < chunk.size(); i += 4*sizeof(float)) // 4 floats per DIG
                {
                    materialPass.addDIG(W3DRGBA(stream));
                }
                break;
            }
            case W3D_CHUNK::eSCG:
            {
                for(size_t i=0; i < chunk.size(); i += 4*sizeof(float)) // 4 floats per SCG
                {
                    materialPass.addSCG(W3DRGBA(stream));
                }
                break;
            }
            case W3D_CHUNK::eTEXTURE_STAGE:
            {
                materialPass.addTextureStage(ReadTextureStage(stream, chunk.size()));
                break;
            }
            default:
            {
                spdlog::debug("Unknown material pass chunk type: 0x{:8x}, skipping...", (uint32_t)chunk.type());
                stream->Seek(chunk.size(), aiOrigin_CUR);
                break;
            }
        }
    }
    return materialPass;
}

static W3DMesh ReadMesh(Assimp::IOStream *stream, uint32_t chunkSize)
{
    W3DMesh mesh{};

    size_t startOffset = stream->Tell();
    while(stream->Tell() < (startOffset + chunkSize))
    {
        W3DChunkHeader chunk(stream);
        switch(chunk.type())
        {
            case W3D_CHUNK::eMESH_HEADER3:
            {
                mesh.setHeader(W3DMeshHeader3(stream));
                break;
            }
            case W3D_CHUNK::eVERTICES:
            {
                for(uint32_t i = 0; i < mesh.header().numVertices(); i++)
                {
                    mesh.addVertex(W3DVector(stream));
                }
                break;
            }
            case W3D_CHUNK::eVERTEX_NORMALS:
            {
                for(uint32_t i = 0; i < mesh.header().numVertices(); i++)
                {
                    mesh.addNormal(W3DVector(stream));
                }
                break;
            }
            case W3D_CHUNK::eVERTEX_INFLUENCES:
            {
                for(uint32_t i = 0; i < mesh.header().numVertices(); i++)
                {
                    mesh.addInfluence(W3DVertexInfo(stream));
                }
                break;
            }
            case W3D_CHUNK::eTRIANGLES:
            {
                for(uint32_t i=0; i < mesh.header().numTriangles(); i++)
                {
                    mesh.addTriangle(W3DTriangle(stream));
                }
                break;
            }
            case W3D_CHUNK::eVERTEX_SHADE_INDICES:
            {
                uint32_t tmp;
                for(uint32_t i=0; i < mesh.header().numVertices(); i++)
                {
                    stream->Read(&tmp, sizeof(uint32_t), 1);
                    mesh.addShadeIndex(boost::endian::little_to_native(tmp));
                }
                break;
            }
            case W3D_CHUNK::eMATERIAL_INFO:
            {
                mesh.setMaterialInfo(W3DMaterialInfo(stream));
                break;
            }
            case W3D_CHUNK::eVERTEX_MATERIALS:
            {
                for(uint32_t i=0; i < mesh.materialInfo().vertexMaterialCount(); i++)
                {
                    W3DChunkHeader materialChunk(stream);
                    if(materialChunk.type() == W3D_CHUNK::eVERTEX_MATERIAL)
                    {
                        mesh.addMaterial(ReadVertexMaterial(stream, materialChunk.size()));
                    }
                    else
                    {
                        stream->Seek(materialChunk.size(), aiOrigin_CUR);
                    }
                }
                break;
            }
            case W3D_CHUNK::eSHADERS:
            {
                for(uint32_t i=0; i < mesh.materialInfo().shaderCount(); i++)
                {
                    mesh.addShader(W3DShader(stream));
                }
                break;
            }
            case W3D_CHUNK::eTEXTURES:
            {
                for(uint32_t i=0; i < mesh.materialInfo().textureCount(); i++)
                {
                    W3DChunkHeader textureChunk(stream);
                    if(textureChunk.type() == W3D_CHUNK::eTEXTURE)
                    {
                        mesh.addTexture(ReadTexture(stream, textureChunk.size()));
                    }
                    else
                    {
                        stream->Seek(textureChunk.size(), aiOrigin_CUR);
                    }
                }
                break;
            }
            case W3D_CHUNK::eMATERIAL_PASS:
            {
                mesh.addMaterialPass(ReadMaterialPass(stream, chunk.size()));
                break;
            }
            default:
            {
                stream->Seek(chunk.size(), aiOrigin_CUR);
                break;
            }
        }
    }
    return mesh;
}

W3DFile W3DLoader::Load(Assimp::IOStream *stream)
{
    W3DFile w3dfile{};

    while(stream->Tell() < stream->FileSize())
    {
        W3DChunkHeader chunk(stream);

        switch (chunk.type())
        {
            case W3D_CHUNK::eHIERARCHY:
            {
                try
                {
                    w3dfile.setHierarchy(ReadHierarchy(stream, chunk.size()));
                }
                catch(const std::exception &e)
                {
                    spdlog::error("Failed to read hierarchy: {}", e.what());
                }
                break;
            }
            case W3D_CHUNK::eMESH:
            {
                try
                {
                    w3dfile.addMesh(ReadMesh(stream, chunk.size()));
                }
                catch(const std::exception &e)
                {
                    spdlog::error("Failed to read mesh: {}", e.what());
                }
                break;
            }
            // TODO: load animation
            // case W3D_CHUNK::eANIMATION:
            //     // ReadAnimation
            //     spdlog::debug("Reading chunk model animation");
            //     break;
            // TODO: load HLOD
            // case W3D_CHUNK::eHLOD:
            //     // ReadHLOD
            //     spdlog::debug("Reading chunk model HLOD");
            //     break;
            default:
            {
                stream->Seek(chunk.size(), aiOrigin_CUR);
                break;
            }
        }
    }
    return w3dfile;
}

void W3DFile::setHierarchy(const W3DHierarchy& hierarchy)
{
    m_hierarchy = hierarchy;
}

const W3DHierarchy& W3DFile::getHierarchy() const
{
    return m_hierarchy;
}

void W3DFile::addMesh(const W3DMesh& mesh)
{
    m_meshes.emplace_back(mesh);
}

size_t W3DFile::meshCount() const
{
    return m_meshes.size();
}

const W3DMesh& W3DFile::mesh(size_t index) const
{
    if(index >= m_meshes.size())
    {
        throw std::out_of_range("Mesh index out of range");
    }
    return m_meshes[index];
}

void W3DFile::addAnimation(const W3DAnimation& animation)
{
    m_animations.emplace_back(animation);
}

size_t W3DFile::animationCount() const
{
    return m_animations.size();
}

const W3DAnimation& W3DFile::animation(size_t index) const
{
    if(index >= m_animations.size())
    {
        throw std::out_of_range("Animation index out of range");
    }
    return m_animations[index];
}