#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <memory>

#include "common/loaders/basicresourceloader.hpp"

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    void init();
    void terminate();

    bool contains(const std::string &path) const;
    std::shared_ptr<DataResource> get(const std:: string &path, bool enableCaching=false) const;
private:
    std::vector<std::shared_ptr<BasicResourceLoader>> m_resourceLoaders;
};

#endif // RESOURCEMANAGER_HPP
