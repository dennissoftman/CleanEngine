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

    void addMaterial(Material *mat, const std::string &name);

    Material *getMaterial(const std::string &name) const;

protected:
    std::unordered_map<std::string, Material*> m_materials;
};

#endif // MATERIALMANAGER_HPP
