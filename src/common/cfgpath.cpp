#include "common/cfgpath.hpp"
#include <stdexcept>
#include <filesystem>

extern "C" {
#include <cfgpath.h>
}

std::string CfgPath::configFilePath(const std::string &appName)
{
    char buff[MAX_PATH];
    get_user_config_file(buff, MAX_PATH, appName.c_str());
    if(buff[0] == '\0')
        throw std::runtime_error("failed to determine config file path");
    return std::string(buff);
}

std::string CfgPath::configDirectoryPath(const std::string &appName)
{
    char buff[MAX_PATH];
    get_user_config_folder(buff, MAX_PATH, appName.c_str());
    if(buff[0] == '\0')
        throw std::runtime_error("failed to determine config directory path");
    std::string path = buff;
    if(!std::filesystem::exists(path))
        std::filesystem::create_directory(path);
    return std::string(buff);
}

std::string CfgPath::cacheDirectoryPath(const std::string &appName)
{
    char buff[MAX_PATH];
    get_user_cache_folder(buff, MAX_PATH, appName.c_str());
    if(buff[0] == '\0')
        throw std::runtime_error("failed to determine cache directory path");
    std::string path = buff;
    if(!std::filesystem::exists(path))
        std::filesystem::create_directory(path);
    return path;
}
