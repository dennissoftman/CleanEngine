#include <minizip/unzip.h>
#include <stdexcept>
#include <fmt/format.h>
#include <boost/algorithm/string.hpp>
#include <spdlog/spdlog.h>

#include "common/loaders/zipresourceloader.hpp"

ZipResourceLoader::ZipResourceLoader(const std::vector<std::string>& zipFiles)
    : m_zipFiles(zipFiles)
{

}

bool ZipResourceLoader::contains(const std::string& path) const
{
    for(auto& zipFile : m_zipFiles)
    {
        unzFile zip = unzOpen(zipFile.c_str());
        if(zip)
        {
            unz_file_info info{};
            char filename[FILENAME_MAX];
            unzGoToFirstFile(zip);
            do
            {
                unzGetCurrentFileInfo(zip, &info, filename, sizeof(filename), nullptr, 0, nullptr, 0);
                if(boost::iequals(std::string(filename), path))
                {
                    unzClose(zip);
                    return true;
                }
            } while(unzGoToNextFile(zip) == UNZ_OK);
            unzClose(zip);
        }
    }
    return false;
}

void ZipResourceLoader::addZipFile(const std::string& path)
{
    m_zipFiles.emplace_back(path);
}

std::shared_ptr<DataResource> ZipResourceLoader::get(const std::string& path, bool caching) const
{
    // TODO: caching
    for(auto& zipFile : m_zipFiles)
    {
        unzFile zip = unzOpen(zipFile.c_str());
        if(zip)
        {
            unz_file_info info{};
            char filename[FILENAME_MAX];
            unzGoToFirstFile(zip);
            do
            {
                unzGetCurrentFileInfo(zip, &info, filename, sizeof(filename), nullptr, 0, nullptr, 0);
                if(boost::iequals(std::string(filename), path))
                {
                    unzOpenCurrentFile(zip);
                    auto resource = std::make_shared<DataResource>();
                    resource->data.resize(info.uncompressed_size);
                    unzReadCurrentFile(zip, resource->data.data(), info.uncompressed_size);
                    unzCloseCurrentFile(zip);
                    unzClose(zip);
                    return resource;
                }
            } while(unzGoToNextFile(zip) == UNZ_OK);
            unzClose(zip);
        }
    }
    throw std::runtime_error(fmt::format("Resource '{}' not found", path));
}

const char* ZipResourceLoader::getType() const
{
    return "ZipResourceLoader";
}