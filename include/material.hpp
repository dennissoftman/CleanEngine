#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm/mat4x4.hpp>
#include <string>

#include "imageloader.hpp"

struct TransformData
{
    alignas(16) glm::mat4 Projection;
    alignas(16) glm::mat4 View;
    alignas(16) glm::mat4 Model;
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

    virtual void setImage(const ImageData &imgData, const std::string &name) = 0;
    virtual void loadImage(const std::string &path, const std::string &name) = 0;
    virtual void setColor(const glm::vec4 &color, const std::string &name) = 0;

    // material properties
    virtual void setDoubleSided(bool yes) = 0;
    virtual bool isDoubleSided() const = 0;
private:
};

#endif // MATERIAL_HPP
