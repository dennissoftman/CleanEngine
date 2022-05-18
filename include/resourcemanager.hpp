#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <string>

class ResourceManager
{
public:
    ResourceManager();

    /*
     * Converts requested path to engine path
     * Example:
     *      data/textures/file.png => /path/to/engine/data/textures/file.png
     *
    */
    std::string getEnginePath(const std::string &path);

private:
};

#endif // RESOURCEMANAGER_HPP
