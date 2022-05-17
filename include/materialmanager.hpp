#ifndef MATERIALMANAGER_HPP
#define MATERIALMANAGER_HPP

#include "material.hpp"

#include <string>
#include <unordered_map>

class MaterialManager
{
public:
    MaterialManager();
    ~MaterialManager();

    void addMaterial(const std::string &name, Material *mat);

    Material *get(const std::string &name) const;

protected:
    std::unordered_map<std::string, Material*> m_materials;
};

#endif // MATERIALMANAGER_HPP
