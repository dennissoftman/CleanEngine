#include <spdlog/spdlog.h>

#include "common/resourcemanager.hpp"
#include "common/servicelocator.hpp"

#include "common/loaders/fileresourceloader.hpp"
#include "common/loaders/zipresourceloader.hpp"
#include "common/loaders/bigresourceloader.hpp"


ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
    terminate();
}

void ResourceManager::init()
{
    m_resourceLoaders.emplace_back(std::make_shared<FileResourceLoader>(std::vector{std::string("./data/")}));
    // TODO: iterate through data folder and enumerate all zip files (.gar, .jar, .zip, etc.)
    m_resourceLoaders.emplace_back(std::make_shared<ZipResourceLoader>(std::vector{std::string("./data/base.gar")}));
    // TEMPORARY: hardcode the path to the INI.big file
    m_resourceLoaders.emplace_back(std::make_shared<BigResourceLoader>(std::vector{
        std::string("D:\\Games\\Command and Conquer Generals Zero Hour\\Command and Conquer Generals\\INI.big"),
        std::string("D:\\Games\\Command and Conquer Generals Zero Hour\\Command and Conquer Generals\\Textures.big"),
        std::string("D:\\Games\\Command and Conquer Generals Zero Hour\\Command and Conquer Generals\\W3D.big"),
        std::string("D:\\Games\\Command and Conquer Generals Zero Hour\\Command and Conquer Generals Zero Hour\\CWC\\_499_CWC.cwc")}));

    spdlog::debug("Resource manager initialized with {} loaders", m_resourceLoaders.size());
}

void ResourceManager::terminate()
{
    m_resourceLoaders.clear();
}

bool ResourceManager::contains(const std::string &path) const
{
    for(auto &loader : m_resourceLoaders)
    {
        if(loader->contains(path))
            return true;
    }
    return false;
}

std::shared_ptr<DataResource> ResourceManager::get(const std::string &path, bool enableCaching) const
{
    for(auto &loader : m_resourceLoaders)
    {
        if(loader->contains(path))
            return loader->get(path, enableCaching);
    }
    throw std::runtime_error(fmt::format("Resource '{}' not found", path));
}
