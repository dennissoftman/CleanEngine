#include "client/materialmanager.hpp"
#include "common/servicelocator.hpp"

MaterialManager::MaterialManager()
{

}

MaterialManager::~MaterialManager()
{

}

void MaterialManager::addMaterial(Material *mat, const std::string &name)
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
