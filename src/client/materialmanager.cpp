#include <filesystem>

#include "client/materialmanager.hpp"
#include "common/servicelocator.hpp"

MaterialManager::MaterialManager()
{

}

MaterialManager::~MaterialManager()
{

}


size_t MaterialManager::createMaterial(const glm::vec3& color, const std::string& name)
{
    return ServiceLocator::getRenderer().CreateColorMaterial(color, name);
}

size_t MaterialManager::createMaterial(const std::string& texture, const std::string& name)
{
    return ServiceLocator::getRenderer().CreateTextureMaterial(texture, name);
}