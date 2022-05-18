#include "resourcemanager.hpp"
#include <cstdlib>

ResourceManager::ResourceManager()
{

}

std::string ResourceManager::getEnginePath(const std::string &path)
{
    const char *res = std::getenv("CLEAN_ENGINE_HOME");
    if(res)
        return std::string(res) + "/" + path;
    return "./"+path;
}
