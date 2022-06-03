#include "client/materialmanager.hpp"
#include "common/servicelocator.hpp"

MaterialManager::MaterialManager()
{

}

MaterialManager::~MaterialManager()
{

}

void MaterialManager::loadImage(const DataResource &imgResource, const std::string &name)
{
    if(getMaterial(name) == nullptr)
    {
        Material *mat = Material::create();
        ImageData imgData = ImageLoader::loadImageMemory(imgResource.data.get(), imgResource.size);
        mat->setImage(imgData, name);
        mat->init();
        addMaterial(name, mat);
    }
}

void MaterialManager::loadColor(const glm::vec4 &color, const std::string &name)
{
    if(getMaterial(name) == nullptr)
    {
        Material *mat = Material::create();
        mat->setColor(color, "color");
        mat->init();
        addMaterial(name, mat);
    }
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
