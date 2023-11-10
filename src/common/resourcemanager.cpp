#include <fstream>
#include <filesystem>
#include <cstring>
#include <minizip/unzip.h>
#include <spdlog/spdlog.h>

#include "common/resourcemanager.hpp"
#include "common/servicelocator.hpp"

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
                unzFile fp = unzOpen(fpath.c_str());
                if(fp)
                {
                    m_resourceArchives.emplace(fpath, fp);
                }
                else
                {
                    spdlog::error("Failed to open archive: '"+fpath+"'");
                }
            }
        }
    }
}

void ResourceManager::terminate()
{
    m_cachedResources.clear();
    for(auto &kv : m_resourceArchives)
        unzClose(kv.second);
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
            unzFile arcFile = arc.second;

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

            res.data = std::make_shared<unsigned char[]>(res.size);
            found = true;
            unzReadCurrentFile(arcFile, res.data.get(), res.size);

            unzCloseCurrentFile(arcFile); // close file in archive
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
        res.data = std::make_shared<unsigned char[]>(res.size);
        fp.read(reinterpret_pointer_cast<char[]>(res.data).get(), res.size);
        fp.close();
    }

    if(enableCaching)
        m_cachedResources[path] = res;
    return res;
}
