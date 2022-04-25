#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm/mat4x4.hpp>
#include <string>

struct TransformData
{
    glm::mat4 Projection, View, Model;

};

class Material
{
public:
    // create shader and init fields
    virtual void init() = 0;

    // execute before draw calls
    virtual void use(const TransformData &data) = 0;

    virtual void setImage(const std::string &name, const std::string &path) = 0;

    // material properties
    virtual void setDoubleSided(bool yes) = 0;
    virtual bool isDoubleSided() const = 0;
private:
};

#endif // MATERIAL_HPP
