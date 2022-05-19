#include "materialmanager.hpp"

MaterialManager::MaterialManager()
{

}

MaterialManager::~MaterialManager()
{

}

void MaterialManager::addMaterial(const std::string &name, Material *mat)
{
    if(!m_materials.contains(name))
    {
        if(mat)
            m_materials[name] = mat;
    }
}

Material *MaterialManager::getMaterial(const std::string &name) const
{
    if(m_materials.contains(name))
        return m_materials.at(name);
    return nullptr;
}
