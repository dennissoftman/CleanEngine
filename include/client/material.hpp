#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "imageloader.hpp"

#include <glm/mat4x4.hpp>
#include <string>

struct TransformData
{
    alignas(16) glm::mat4 Projection;
    alignas(16) glm::mat4 View;
    alignas(16) glm::mat4 Model;
};

struct LightingData
{
    glm::vec4 lightPositions[16];
    glm::vec4 lightColors[16];
    glm::vec4 viewPos=glm::vec4(0, 1, 0, 1);
    int lightCount=1;
};

class RenderData
{
public:
    virtual std::string getType() const = 0;
};

class Material
{
public:
    static Material *create();
    virtual ~Material() {}

    // create shader and init fields
    virtual void init() = 0;

    // execute before draw calls
    virtual void use(TransformData &transformData) = 0;

    // set physical based rendering textures (specular workflow)
    virtual void setPBR(const ImageData &albedo, const ImageData &normal,
                        const ImageData &roughness, const ImageData &metallic,
                        const ImageData &ambient) = 0;

    virtual void setImage(const ImageData &imgData, const std::string &name) = 0;
    virtual void setColor(const glm::vec4 &color, const std::string &name) = 0;

    // material properties
    virtual void setDoubleSided(bool yes) = 0;
    virtual bool isDoubleSided() const = 0;

    enum MaterialMode
    {
        eNone,
        eColor,
        eDiffuse,
        ePBR
    };

    enum TextureType
    {
        eAlbedo,
        eNormal,
        eRoughness,
        eMetallic,
        eAmbientOcclusion,
        eHeight,
    };
};

#endif // MATERIAL_HPP
