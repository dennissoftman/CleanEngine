#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>

#include "common/loaders/fileresourceloader.hpp"

FileResourceLoader::FileResourceLoader(const std::vector<std::string>& paths)
    : m_paths(paths)
{

}

bool FileResourceLoader::contains(const std::string& path) const
{
    for(auto& dir : m_paths)
    {
        if(std::filesystem::exists(std::filesystem::path(dir) / path))
            return true;
    }
    return false;
}

std::shared_ptr<DataResource> FileResourceLoader::get(const std::string& path, bool caching) const
{
    if(!contains(path))
    {
        throw std::runtime_error(fmt::format("File '{}' not found", path));
    }
    // TODO: caching
    auto res = std::make_shared<DataResource>();
    for(auto& dir : m_paths)
    {
        std::ifstream file(std::filesystem::path(dir) / path, std::ios::binary);
        if(file)
        {
            file.seekg(0, std::ios::end);
            res->data.resize(file.tellg());
            file.seekg(0, std::ios::beg);
            file.read(res->data.data(), res->data.size());
        }
        else
        {
            throw std::runtime_error(fmt::format("File '{}' could not be opened", path));
        }
    }
    return res;
}

const char* FileResourceLoader::getType() const
{
    return "FileResourceLoader";
}