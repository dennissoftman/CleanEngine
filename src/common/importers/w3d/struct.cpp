#include <assimp/IOStream.hpp>
#include <boost/endian.hpp>
#include <fmt/format.h>

#include "common/importers/w3d/struct.hpp"

W3DChunkHeader::W3DChunkHeader(Assimp::IOStream *stream)
{
    uint32_t tmp;
    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_type = (W3D_CHUNK)boost::endian::little_to_native(tmp);
    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_size = boost::endian::little_to_native(tmp) & 0x7FFFFFFF;
}

W3D_CHUNK W3DChunkHeader::type() const
{
    return m_type;
}

uint32_t W3DChunkHeader::size() const
{
    return m_size;
}

W3DTexCoord::W3DTexCoord(Assimp::IOStream *stream)
{
    float tmp; // just in case someone uses double for texture coordinates
    stream->Read(&tmp, sizeof(float), 1);
    m_texCoord.x = tmp;
    stream->Read(&tmp, sizeof(float), 1);
    m_texCoord.y = tmp;
}

W3DTexCoord::W3DTexCoord(float u, float v)
    : m_texCoord(u, v)
{
}

const glm::vec2& W3DTexCoord::texCoord() const
{
    return m_texCoord;
}

W3DTexCoord::operator std::string() const
{
    return fmt::format("({:.2f}; {:.2f})", m_texCoord.x, m_texCoord.y);
}

W3DVector::W3DVector(Assimp::IOStream *stream)
{
    float tmp; // just in case someone uses double for vectors
    stream->Read(&tmp, sizeof(float), 1);
    m_vector.x = tmp;
    stream->Read(&tmp, sizeof(float), 1);
    m_vector.y = tmp;
    stream->Read(&tmp, sizeof(float), 1);
    m_vector.z = tmp;
}

W3DVector::W3DVector(float x, float y, float z)
    : m_vector(x, y, z)
{
}

const glm::vec3& W3DVector::vector() const
{
    return m_vector;
}

W3DVector::operator std::string() const
{
    return fmt::format("({:.2f}; {:.2f}; {:.2f})", m_vector.x, m_vector.y, m_vector.z);
}

W3DVectori::W3DVectori(Assimp::IOStream *stream)
{
    int32_t tmp;
    stream->Read(&tmp, sizeof(int32_t), 1);
    m_vector.x = boost::endian::little_to_native(tmp);
    stream->Read(&tmp, sizeof(int32_t), 1);
    m_vector.y = boost::endian::little_to_native(tmp);
    stream->Read(&tmp, sizeof(int32_t), 1);
    m_vector.z = boost::endian::little_to_native(tmp);
}

W3DVectori::W3DVectori(int32_t x, int32_t y, int32_t z)
    : m_vector(x, y, z)
{
}

const glm::ivec3& W3DVectori::vector() const
{
    return m_vector;
}

W3DVectori::operator std::string() const
{
    return fmt::format("({}; {}; {})", m_vector.x, m_vector.y, m_vector.z);
}

W3DQuaternion::W3DQuaternion(Assimp::IOStream *stream)
{
    float tmp; // just in case someone uses double for quaternions
    stream->Read(&tmp, sizeof(float), 1);
    m_quaternion.x = tmp;
    stream->Read(&tmp, sizeof(float), 1);
    m_quaternion.y = tmp;
    stream->Read(&tmp, sizeof(float), 1);
    m_quaternion.z = tmp;
    stream->Read(&tmp, sizeof(float), 1);
    m_quaternion.w = tmp;
}

W3DQuaternion::W3DQuaternion(float x, float y, float z, float w)
    : m_quaternion(x, y, z, w)
{
}

const glm::quat& W3DQuaternion::quaternion() const
{
    return m_quaternion;
}

W3DQuaternion::operator std::string() const
{
    return fmt::format("({:.2f}; {:.2f}; {:.2f}; {:.2f})", m_quaternion.x, m_quaternion.y, m_quaternion.z, m_quaternion.w);
}

W3DRGB::W3DRGB(Assimp::IOStream *stream)
{
    stream->Read(&m_rgb.r, 1, 3);  // yes I am just reading 3 bytes using pointer magic in C++20
    stream->Seek(1, aiOrigin_CUR); // padding (alpha)
}

W3DRGB::W3DRGB(uint8_t r, uint8_t g, uint8_t b)
    : m_rgb(r, g, b)
{
}

const glm::u8vec3& W3DRGB::rgb() const
{
    return m_rgb;
}

W3DRGB::operator std::string() const
{
    return fmt::format("({}; {}; {})", m_rgb.r, m_rgb.g, m_rgb.b);
}

W3DRGBA::W3DRGBA(Assimp::IOStream *stream)
{
    stream->Read(&m_rgba.r, 1, 4); // yes I am just reading 4 bytes using pointer magic in C++20
}

W3DRGBA::W3DRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : m_rgba(r, g, b, a)
{
}

const glm::u8vec4& W3DRGBA::rgba() const
{
    return m_rgba;
}

W3DRGBA::operator std::string() const
{
    return fmt::format("({}; {}; {}; {})", m_rgba.r, m_rgba.g, m_rgba.b, m_rgba.a);
}

W3DMaterialInfo::W3DMaterialInfo(Assimp::IOStream *stream)
{
    uint32_t tmp;
    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_passCount = boost::endian::little_to_native(tmp);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_vertexMaterialCount = boost::endian::little_to_native(tmp);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_shaderCount = boost::endian::little_to_native(tmp);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_textureCount = boost::endian::little_to_native(tmp);
}

uint32_t W3DMaterialInfo::passCount() const
{
    return m_passCount;
}

uint32_t W3DMaterialInfo::vertexMaterialCount() const
{
    return m_vertexMaterialCount;
}

uint32_t W3DMaterialInfo::shaderCount() const
{
    return m_shaderCount;
}

uint32_t W3DMaterialInfo::textureCount() const
{
    return m_textureCount;
}

W3DVertexMaterial::W3DVertexMaterial(Assimp::IOStream *stream)
{
    uint32_t tmp;
    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_attributes = boost::endian::little_to_native(tmp);

    m_ambient = W3DRGB(stream);

    m_diffuse = W3DRGB(stream);

    m_specular = W3DRGB(stream);

    m_emissive = W3DRGB(stream);

    stream->Read(&m_shininess, sizeof(float), 1);

    stream->Read(&m_opacity, sizeof(float), 1);

    stream->Read(&m_translucency, sizeof(float), 1);
}

W3DShader::W3DShader(Assimp::IOStream *stream)
{
    stream->Read(&m_depthCompare, 1, 1);
    stream->Read(&m_depthMask, 1, 1);
    stream->Read(&m_colorMask, 1, 1);
    stream->Read(&m_destBlend, 1, 1);
    stream->Read(&m_fogFunc, 1, 1);
    stream->Read(&m_priGradient, 1, 1);
    stream->Read(&m_secGradient, 1, 1);
    stream->Read(&m_srcBlend, 1, 1);
    stream->Read(&m_texturing, 1, 1);
    stream->Read(&m_detailColorFunc, 1, 1);
    stream->Read(&m_detailAlphaFunc, 1, 1);
    stream->Read(&m_shaderPreset, 1, 1);
    stream->Read(&m_alphaTest, 1, 1);
    stream->Read(&m_postDetailColorFunc, 1, 1);
    stream->Read(&m_postDetailAlphaFunc, 1, 1);
    stream->Seek(1, aiOrigin_CUR); // padding
}

uint8_t W3DShader::depthCompare() const
{
    return m_depthCompare;
}

uint8_t W3DShader::depthMask() const
{
    return m_depthMask;
}

uint8_t W3DShader::colorMask() const
{
    return m_colorMask;
}

uint8_t W3DShader::destBlend() const
{
    return m_destBlend;
}

uint8_t W3DShader::fogFunc() const
{
    return m_fogFunc;
}

uint8_t W3DShader::priGradient() const
{
    return m_priGradient;
}

uint8_t W3DShader::secGradient() const
{
    return m_secGradient;
}

uint8_t W3DShader::srcBlend() const
{
    return m_srcBlend;
}

uint8_t W3DShader::texturing() const
{
    return m_texturing;
}

uint8_t W3DShader::detailColorFunc() const
{
    return m_detailColorFunc;
}

uint8_t W3DShader::detailAlphaFunc() const
{
    return m_detailAlphaFunc;
}

uint8_t W3DShader::shaderPreset() const
{
    return m_shaderPreset;
}

uint8_t W3DShader::alphaTest() const
{
    return m_alphaTest;
}

uint8_t W3DShader::postDetailColorFunc() const
{
    return m_postDetailColorFunc;
}

uint8_t W3DShader::postDetailAlphaFunc() const
{
    return m_postDetailAlphaFunc;
}

W3DTextureInfo::W3DTextureInfo(Assimp::IOStream *stream)
{
    uint16_t tmp16;
    uint32_t tmp32;

    stream->Read(&tmp16, sizeof(uint16_t), 1);
    m_attributes = boost::endian::little_to_native(tmp16);

    stream->Read(&tmp16, sizeof(uint16_t), 1);
    m_animType = boost::endian::little_to_native(tmp16);

    stream->Read(&tmp32, sizeof(uint32_t), 1);
    m_frameCount = boost::endian::little_to_native(tmp32);

    stream->Read(&m_frameRate, sizeof(float), 1);
}

uint16_t W3DTextureInfo::attributes() const
{
    return m_attributes;
}

uint16_t W3DTextureInfo::animType() const
{
    return m_animType;
}

uint32_t W3DTextureInfo::frameCount() const
{
    return m_frameCount;
}

float W3DTextureInfo::frameRate() const
{
    return m_frameRate;
}

W3DTriangle::W3DTriangle(Assimp::IOStream *stream)
{
    uint32_t tmp;
    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_vertexIndices[0] = boost::endian::little_to_native(tmp);
    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_vertexIndices[1] = boost::endian::little_to_native(tmp);
    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_vertexIndices[2] = boost::endian::little_to_native(tmp);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_attributes = boost::endian::little_to_native(tmp);

    m_normal = W3DVector(stream);

    stream->Read(&m_dist, sizeof(float), 1);
}

uint32_t W3DTriangle::vertexIndex(uint32_t index) const
{
    return m_vertexIndices[index%3];
}

uint32_t W3DTriangle::attributes() const
{
    return m_attributes;
}

const W3DVector& W3DTriangle::normal() const
{
    return m_normal;
}

float W3DTriangle::dist() const
{
    return m_dist;
}

W3DMeshHeader3::W3DMeshHeader3(Assimp::IOStream *stream)
{
    uint32_t tmp;
    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_version = boost::endian::little_to_native(tmp);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_attributes = boost::endian::little_to_native(tmp);

    char nameBuff[W3D_NAME_LEN];
    stream->Read(nameBuff, 1, W3D_NAME_LEN);
    m_meshName = std::string(nameBuff, W3D_NAME_LEN);

    stream->Read(nameBuff, 1, W3D_NAME_LEN);
    m_containerName = std::string(nameBuff, W3D_NAME_LEN);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_numTriangles = boost::endian::little_to_native(tmp);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_numVertices = boost::endian::little_to_native(tmp);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_numMaterials = boost::endian::little_to_native(tmp);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_numDamageStages = boost::endian::little_to_native(tmp);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_sortLevel = (int32_t)tmp;

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_prelitVersion = boost::endian::little_to_native(tmp);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_futureCounts[0] = boost::endian::little_to_native(tmp);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_vertexChannels = boost::endian::little_to_native(tmp);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_faceChannels = boost::endian::little_to_native(tmp);

    m_boundingBoxMin = W3DVector(stream);

    m_boundingBoxMax = W3DVector(stream);

    m_boundingSphereCenter = W3DVector(stream);

    stream->Read(&m_boundingSphereRadius, sizeof(float), 1);
}

uint32_t W3DMeshHeader3::version() const
{
    return m_version;
}

uint32_t W3DMeshHeader3::attributes() const
{
    return m_attributes;
}

const std::string& W3DMeshHeader3::meshName() const
{
    return m_meshName;
}

const std::string& W3DMeshHeader3::containerName() const
{
    return m_containerName;
}

uint32_t W3DMeshHeader3::numTriangles() const
{
    return m_numTriangles;
}

uint32_t W3DMeshHeader3::numVertices() const
{
    return m_numVertices;
}

uint32_t W3DMeshHeader3::numMaterials() const
{
    return m_numMaterials;
}

uint32_t W3DMeshHeader3::numDamageStages() const
{
    return m_numDamageStages;
}

int32_t W3DMeshHeader3::sortLevel() const
{
    return m_sortLevel;
}

uint32_t W3DMeshHeader3::prelitVersion() const
{
    return m_prelitVersion;
}

uint32_t W3DMeshHeader3::futureCounts() const
{
    return m_futureCounts[0];
}

uint32_t W3DMeshHeader3::vertexChannels() const
{
    return m_vertexChannels;
}

uint32_t W3DMeshHeader3::faceChannels() const
{
    return m_faceChannels;
}

const W3DVector& W3DMeshHeader3::boundingBoxMin() const
{
    return m_boundingBoxMin;
}

const W3DVector& W3DMeshHeader3::boundingBoxMax() const
{
    return m_boundingBoxMax;
}

const W3DVector& W3DMeshHeader3::boundingSphereCenter() const
{
    return m_boundingSphereCenter;
}

float W3DMeshHeader3::boundingSphereRadius() const
{
    return m_boundingSphereRadius;
}

W3DVertexInfo::W3DVertexInfo(Assimp::IOStream *stream)
{
    uint16_t tmp;
    stream->Read(&tmp, sizeof(uint16_t), 1);
    m_boneIndex = boost::endian::little_to_native(tmp);

    stream->Seek(6, aiOrigin_CUR); // padding
}

uint16_t W3DVertexInfo::boneIndex() const
{
    return m_boneIndex;
}

W3DMeshDeform::W3DMeshDeform(Assimp::IOStream *stream)
{
    uint32_t tmp;
    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_setCount = boost::endian::little_to_native(tmp);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_alphaPasses = boost::endian::little_to_native(tmp);

    stream->Seek(12, aiOrigin_CUR); // reserved
}

uint32_t W3DMeshDeform::setCount() const
{
    return m_setCount;
}

uint32_t W3DMeshDeform::alphaPasses() const
{
    return m_alphaPasses;
}

W3DDeformSetInfo::W3DDeformSetInfo(Assimp::IOStream *stream)
{
    uint32_t tmp;
    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_keyframeCount = boost::endian::little_to_native(tmp);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_flags = boost::endian::little_to_native(tmp);

    stream->Seek(4, aiOrigin_CUR); // reserved
}

uint32_t W3DDeformSetInfo::keyframeCount() const
{
    return m_keyframeCount;
}

uint32_t W3DDeformSetInfo::flags() const
{
    return m_flags;
}

W3DDeformKeyframeInfo::W3DDeformKeyframeInfo(Assimp::IOStream *stream)
{
    stream->Read(&m_deformPercent, sizeof(float), 1);

    uint32_t tmp;
    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_dataCount = boost::endian::little_to_native(tmp);

    stream->Seek(8, aiOrigin_CUR); // reserved
}

float W3DDeformKeyframeInfo::deformPercent() const
{
    return m_deformPercent;
}

uint32_t W3DDeformKeyframeInfo::dataCount() const
{
    return m_dataCount;
}

W3DDeformData::W3DDeformData(Assimp::IOStream *stream)
{
    uint32_t tmp;
    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_vertexIndex = boost::endian::little_to_native(tmp);

    m_position = W3DVector(stream);

    m_color = W3DRGBA(stream);
}

W3DHierarchyHeader::W3DHierarchyHeader(Assimp::IOStream *stream)
{
    uint32_t tmp;
    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_version = boost::endian::little_to_native(tmp);

    char nameBuff[W3D_NAME_LEN];
    stream->Read(nameBuff, 1, W3D_NAME_LEN);
    m_name = std::string(nameBuff, W3D_NAME_LEN);

    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_numPivots = boost::endian::little_to_native(tmp);

    m_center = W3DVector(stream);
}

uint32_t W3DHierarchyHeader::version() const
{
    return m_version;
}

const std::string& W3DHierarchyHeader::name() const
{
    return m_name;
}

uint32_t W3DHierarchyHeader::numPivots() const
{
    return m_numPivots;
}

const W3DVector& W3DHierarchyHeader::center() const
{
    return m_center;
}

W3DHierarchyHeader::operator std::string() const
{
    return fmt::format("W3DHierarchyHeader {{ version: {:8x}, name: {}, numPivots: {}, center: {} }}",
        m_version, m_name, m_numPivots, static_cast<std::string>(m_center));
}

W3DPivot::W3DPivot(Assimp::IOStream *stream)
{
    char nameBuff[W3D_NAME_LEN];
    stream->Read(nameBuff, 1, W3D_NAME_LEN);
    m_name = std::string(nameBuff, W3D_NAME_LEN);

    uint32_t tmp;
    stream->Read(&tmp, sizeof(uint32_t), 1);
    m_parentIndex = boost::endian::little_to_native(tmp);

    m_translation = W3DVector(stream);

    m_eulerAngles = W3DVector(stream);

    m_rotation = W3DQuaternion(stream);

    m_valid = true;
}

const std::string& W3DPivot::name() const
{
    return m_name;
}

uint32_t W3DPivot::parentIndex() const
{
    return m_parentIndex;
}

const W3DVector& W3DPivot::translation() const
{
    return m_translation;
}

const W3DVector& W3DPivot::eulerAngles() const
{
    return m_eulerAngles;
}

const W3DQuaternion& W3DPivot::rotation() const
{
    return m_rotation;
}

bool W3DPivot::valid() const
{
    return m_valid;
}

void W3DMaterial::setName(const std::string& name)
{
    m_name = name;
}

const std::string& W3DMaterial::name() const
{
    return m_name;
}

void W3DMaterial::setVertexMaterialInfo(const W3DVertexMaterial& vertexMaterialInfo)
{
    m_vertexMaterialInfo = vertexMaterialInfo;
}

const W3DVertexMaterial& W3DMaterial::vertexMaterialInfo() const
{
    return m_vertexMaterialInfo;
}

void W3DMaterial::setMapperArgs0(const std::string& mapperArgs0)
{
    m_mapperArgs0 = mapperArgs0;
}

const std::string& W3DMaterial::mapperArgs0() const
{
    return m_mapperArgs0;
}

void W3DMaterial::setMapperArgs1(const std::string& mapperArgs1)
{
    m_mapperArgs1 = mapperArgs1;
}

const std::string& W3DMaterial::mapperArgs1() const
{
    return m_mapperArgs1;
}

void W3DMaterial::setUPerSec(float UPerSec)
{
    m_UPerSec = UPerSec;
}

float W3DMaterial::UPerSec() const
{
    return m_UPerSec;
}

void W3DMaterial::setVPerSec(float VPerSec)
{
    m_VPerSec = VPerSec;
}

float W3DMaterial::VPerSec() const
{
    return m_VPerSec;
}

void W3DMaterial::setCurrentU(float currentU)
{
    m_currentU = currentU;
}

float W3DMaterial::currentU() const
{
    return m_currentU;
}

void W3DMaterial::setCurrentV(float currentV)
{
    m_currentV = currentV;
}

float W3DMaterial::currentV() const
{
    return m_currentV;
}

void W3DTexture::setName(const std::string& name)
{
    m_name = name;
}

const std::string& W3DTexture::name() const
{
    return m_name;
}

void W3DTexture::setTextureInfo(const W3DTextureInfo& textureInfo)
{
    m_textureInfo = textureInfo;
    m_textureInfoLoaded = true;
}

const W3DTextureInfo& W3DTexture::textureInfo() const
{
    return m_textureInfo;
}

bool W3DTexture::textureInfoLoaded() const
{
    return m_textureInfoLoaded;
}

void W3DTextureStage::addTextureID(uint32_t textureID)
{
    m_textureIDs.emplace_back(textureID);
}

size_t W3DTextureStage::textureIDCount() const
{
    return m_textureIDs.size();
}

uint32_t W3DTextureStage::textureID(uint32_t index) const
{
    if(index >= m_textureIDs.size())
    {
        throw std::out_of_range("W3DTextureStage::textureID: index out of range");
    }
    return m_textureIDs.at(index);
}

void W3DTextureStage::addPerFaceTexCoordID(const W3DVectori& perFaceTexCoordID)
{
    m_perFaceTexCoordIDs.emplace_back(perFaceTexCoordID);
}

size_t W3DTextureStage::perFaceTexCoordIDCount() const
{
    return m_perFaceTexCoordIDs.size();
}

const W3DVectori& W3DTextureStage::perFaceTexCoordID(uint32_t index) const
{
    if(index >= m_perFaceTexCoordIDs.size())
    {
        throw std::out_of_range("W3DTextureStage::perFaceTexCoordID: index out of range");
    }
    return m_perFaceTexCoordIDs.at(index);
}

void W3DTextureStage::addTexCoord(const W3DTexCoord& texCoord)
{
    m_texCoords.emplace_back(texCoord);
}

size_t W3DTextureStage::texCoordCount() const
{
    return m_texCoords.size();
}

const W3DTexCoord& W3DTextureStage::texCoord(uint32_t index) const
{
    if(index >= m_texCoords.size())
    {
        throw std::out_of_range("W3DTextureStage::texCoord: index out of range");
    }
    return m_texCoords.at(index);
}

void W3DMaterialPass::addVertexMaterialID(uint32_t vertexMaterialID)
{
    m_vertexMaterialIDs.emplace_back(vertexMaterialID);
}

size_t W3DMaterialPass::vertexMaterialIDCount() const
{
    return m_vertexMaterialIDs.size();
}

uint32_t W3DMaterialPass::vertexMaterialID(uint32_t index) const
{
    if(index >= m_vertexMaterialIDs.size())
    {
        throw std::out_of_range("W3DMaterialPass::vertexMaterialID: index out of range");
    }
    return m_vertexMaterialIDs.at(index);
}

void W3DMaterialPass::addShaderID(uint32_t shaderID)
{
    m_shaderIDs.emplace_back(shaderID);
}

size_t W3DMaterialPass::shaderIDCount() const
{
    return m_shaderIDs.size();
}

uint32_t W3DMaterialPass::shaderID(uint32_t index) const
{
    if(index >= m_shaderIDs.size())
    {
        throw std::out_of_range("W3DMaterialPass::shaderID: index out of range");
    }
    return m_shaderIDs.at(index);
}

void W3DMaterialPass::addDCG(const W3DRGBA& dcg)
{
    m_DCGs.emplace_back(dcg);
}

size_t W3DMaterialPass::DCGCount() const
{
    return m_DCGs.size();
}

const W3DRGBA& W3DMaterialPass::DCG(uint32_t index) const
{
    if(index >= m_DCGs.size())
    {
        throw std::out_of_range("W3DMaterialPass::DCG: index out of range");
    }
    return m_DCGs.at(index);
}

void W3DMaterialPass::addDIG(const W3DRGBA& dig)
{
    m_DIGs.emplace_back(dig);
}

size_t W3DMaterialPass::DIGCount() const
{
    return m_DIGs.size();
}

const W3DRGBA& W3DMaterialPass::DIG(uint32_t index) const
{
    if(index >= m_DIGs.size())
    {
        throw std::out_of_range("W3DMaterialPass::DIG: index out of range");
    }
    return m_DIGs.at(index);
}

void W3DMaterialPass::addSCG(const W3DRGBA& scg)
{
    m_SCGs.emplace_back(scg);
}

size_t W3DMaterialPass::SCGCount() const
{
    return m_SCGs.size();
}

const W3DRGBA& W3DMaterialPass::SCG(uint32_t index) const
{
    if(index >= m_SCGs.size())
    {
        throw std::out_of_range("W3DMaterialPass::SCG: index out of range");
    }
    return m_SCGs.at(index);
}

void W3DMaterialPass::addTextureStage(const W3DTextureStage& textureStage)
{
    m_textureStages.emplace_back(textureStage);
}

size_t W3DMaterialPass::textureStageCount() const
{
    return m_textureStages.size();
}

const W3DTextureStage& W3DMaterialPass::textureStage(uint32_t index) const
{
    if(index >= m_textureStages.size())
    {
        throw std::out_of_range("W3DMaterialPass::textureStage: index out of range");
    }
    return m_textureStages.at(index);
}

void W3DMesh::setHeader(const W3DMeshHeader3& header)
{
    m_header = header;
}

const W3DMeshHeader3& W3DMesh::header() const
{
    return m_header;
}

void W3DMesh::addVertex(const W3DVector& vertex)
{
    m_vertices.emplace_back(vertex);
}

const W3DVector& W3DMesh::vertex(uint32_t index) const
{
    if(index >= m_vertices.size())
    {
        throw std::out_of_range("W3DMesh::vertex: index out of range");
    }
    return m_vertices.at(index);
}

void W3DMesh::addNormal(const W3DVector& normal)
{
    m_normals.emplace_back(normal);
}

const W3DVector& W3DMesh::normal(uint32_t index) const
{
    if(index >= m_normals.size())
    {
        throw std::out_of_range("W3DMesh::normal: index out of range");
    }
    return m_normals.at(index);
}

void W3DMesh::addInfluence(const W3DVertexInfo& influence)
{
    m_influences.emplace_back(influence);
}

const W3DVertexInfo& W3DMesh::influence(uint32_t index) const
{
    if(index >= m_influences.size())
    {
        throw std::out_of_range("W3DMesh::influence: index out of range");
    }
    return m_influences.at(index);
}

void W3DMesh::addTriangle(const W3DTriangle& triangle)
{
    m_triangles.emplace_back(triangle);
}

const W3DTriangle& W3DMesh::triangle(uint32_t index) const
{
    if(index >= m_triangles.size())
    {
        throw std::out_of_range("W3DMesh::triangle: index out of range");
    }
    return m_triangles.at(index);
}

void W3DMesh::addShadeIndex(uint32_t shadeIndex)
{
    m_shadeIndices.emplace_back(shadeIndex);
}

uint32_t W3DMesh::shadeIndex(uint32_t index) const
{
    if(index >= m_shadeIndices.size())
    {
        throw std::out_of_range("W3DMesh::shadeIndex: index out of range");
    }
    return m_shadeIndices.at(index);
}

void W3DMesh::setMaterialInfo(const W3DMaterialInfo& materialInfo)
{
    m_materialInfo = materialInfo;
}

const W3DMaterialInfo& W3DMesh::materialInfo() const
{
    return m_materialInfo;
}

void W3DMesh::addMaterial(const W3DMaterial& material)
{
    m_materials.emplace_back(material);
}

const W3DMaterial& W3DMesh::material(uint32_t index) const
{
    if(index >= m_materials.size())
    {
        throw std::out_of_range("W3DMesh::material: index out of range");
    }
    return m_materials.at(index);
}

void W3DMesh::addShader(const W3DShader& shader)
{
    m_shaders.emplace_back(shader);
}

const W3DShader& W3DMesh::shader(uint32_t index) const
{
    if(index >= m_shaders.size())
    {
        throw std::out_of_range("W3DMesh::shader: index out of range");
    }
    return m_shaders.at(index);
}

void W3DMesh::addTexture(const W3DTexture& texture)
{
    m_textures.emplace_back(texture);
}

const W3DTexture& W3DMesh::texture(uint32_t index) const
{
    if(index >= m_textures.size())
    {
        throw std::out_of_range("W3DMesh::texture: index out of range");
    }
    return m_textures.at(index);
}

void W3DMesh::addMaterialPass(const W3DMaterialPass& materialPass)
{
    m_materialPasses.emplace_back(materialPass);
}

const W3DMaterialPass& W3DMesh::materialPass(uint32_t index) const
{
    if(index >= m_materialPasses.size())
    {
        throw std::out_of_range("W3DMesh::materialPass: index out of range");
    }
    return m_materialPasses.at(index);
}

void W3DHierarchy::setHeader(const W3DHierarchyHeader& header)
{
    m_header = header;
}

const W3DHierarchyHeader& W3DHierarchy::header() const
{
    return m_header;
}

void W3DHierarchy::addPivot(const W3DPivot& pivot)
{
    m_pivots.emplace_back(pivot);
}

const W3DPivot& W3DHierarchy::pivot(uint32_t index) const
{
    if(index >= m_pivots.size())
    {
        throw std::out_of_range("W3DHierarchy::pivot: index out of range");
    }
    return m_pivots.at(index);
}