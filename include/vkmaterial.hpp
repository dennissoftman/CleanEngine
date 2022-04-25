#ifndef VKMATERIAL_HPP
#define VKMATERIAL_HPP

#include "material.hpp"
#include "vkshader.hpp"

class VkMaterial : public Material
{
public:
    VkMaterial();
    ~VkMaterial();

    void init() override;

    void setImage(const std::string &name, const std::string &path) override;

    void use(const TransformData &data) override;

    void setDoubleSided(bool yes) override;
    bool isDoubleSided() const override;
private:

    bool m_doubleSided;
};

#endif // VKMATERIAL_HPP
