#include "resourcemanager.hpp"
#include "servicelocator.hpp"

#include <fstream>
#include <filesystem>

#include <cstring>
#include <minizip/unzip.h>

static const char *MODULE_NAME = "ResourceManager";

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{
    terminate();
}

void ResourceManager::init()
{
    for(auto &it : std::filesystem::directory_iterator(std::filesystem::current_path()/"data"))
    {
        if(std::filesystem::is_regular_file(it.path()))
        {
            const std::string &fpath = it.path().string();
            if(fpath.ends_with(".gar"))
            {
                m_resourceArchives.push_back(fpath);
            }
        }
    }
}

void ResourceManager::terminate()
{
    m_cachedResources.clear();
    m_resourceArchives.clear();
}

DataResource ResourceManager::getResource(const std::string &path, bool enableCaching)
{
    // simple caching
    if(m_cachedResources.find(path) != m_cachedResources.end())
        return m_cachedResources[path];

    DataResource res{};
    if(path.starts_with(":/")) // archive
    {
        std::string targetPath = path.substr(2); // remove ':/'
        bool found = false;
        for(auto &arc : m_resourceArchives)
        {
            // unzip
            unzFile arcFile = unzOpen(arc.c_str());

            if(unzLocateFile(arcFile, targetPath.c_str(), 0) != UNZ_OK)
                continue;

            unz_file_info fInfo{};
            unzGetCurrentFileInfo(arcFile, &fInfo,
                                  nullptr, 0L,
                                  nullptr, 0L,
                                  nullptr, 0L);

            res.size = fInfo.uncompressed_size;
            if(res.size == 0)
                continue;

            if(unzOpenCurrentFile(arcFile) != UNZ_OK)
                throw std::runtime_error("failed to read archive file '" + targetPath + "'");

            res.data = std::make_shared<char[]>(res.size+1);
            res.data[res.size] = 0; // nul-terminator
            found = true;
            unzReadCurrentFile(arcFile, res.data.get(), res.size);

            unzCloseCurrentFile(arcFile); // close file in archive
            unzClose(arcFile); // close archive
            break;
        }
        if(!found)
            throw std::runtime_error("file '" + path + "' not found");
    }
    else
    {
        std::ifstream fp;
        fp.open(path, std::ios_base::binary);
        if(!fp.is_open())
            throw std::runtime_error("file '" + path + "' not found");
        fp.seekg(0, std::ios_base::end);
        res.size = fp.tellg();
        fp.seekg(0, std::ios_base::beg);
        res.data = std::make_shared<char[]>(res.size+1);
        fp.read(static_pointer_cast<char[]>(res.data).get(), res.size);
        res.data[res.size] = 0; // nul-terminator
        fp.close();
    }

    if(enableCaching)
        m_cachedResources[path] = res;
    return res;
}
