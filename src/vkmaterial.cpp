#include "vkmaterial.hpp"

VkMaterial::VkMaterial()
{

}

VkMaterial::~VkMaterial()
{

}

void VkMaterial::init()
{

}

void VkMaterial::setImage(const std::string &name, const std::string &path)
{

}

void VkMaterial::use(const TransformData &data)
{

}

void VkMaterial::setDoubleSided(bool yes)
{
    m_doubleSided = yes;
}

bool VkMaterial::isDoubleSided() const
{
    return m_doubleSided;
}
