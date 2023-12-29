#ifndef MATERIALMANAGER_HPP
#define MATERIALMANAGER_HPP

#include "common/resourcemanager.hpp"

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class MaterialManager
{
public:
    MaterialManager();
    ~MaterialManager();

    size_t createMaterial(const glm::vec3& color, const std::string& name);
    size_t createMaterial(const std::string& texture, const std::string& name);
};

#endif // MATERIALMANAGER_HPP
