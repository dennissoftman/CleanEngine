#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <vector>
#include <map>
#include <string>
#include <memory>

struct DataResource
{
    std::shared_ptr<char[]> data;
    size_t size;
};

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    void init();
    void terminate();

    DataResource getResource(const std:: string &path, bool enableCaching=true);
private:
    std::map<std::string, DataResource> m_cachedResources;

    std::vector<std::string> m_resourceArchives;
};

#endif // RESOURCEMANAGER_HPP
