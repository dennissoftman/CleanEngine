#ifndef MATERIALMANAGER_HPP
#define MATERIALMANAGER_HPP

#include "client/material.hpp"
#include "common/resourcemanager.hpp"

#include <string>
#include <unordered_map>

class MaterialManager
{
public:
    MaterialManager();
    ~MaterialManager();

    // temp?
    void loadImage(const DataResource &imgResource, const std::string &name);
    void loadColor(const glm::vec4 &color, const std::string &name);
    //

    void addMaterial(const std::string &name, Material *mat);

    Material *getMaterial(const std::string &name) const;

protected:
    std::unordered_map<std::string, Material*> m_materials;
};

#endif // MATERIALMANAGER_HPP