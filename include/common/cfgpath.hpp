#ifndef CFGPATH_HPP
#define CFGPATH_HPP

#include <string>

class CfgPath
{
public:
    /*
     * returns path to single config file (better use 'configDirectoryPath' instead)
    */
    static std::string configFilePath(const std::string &appName);
    /*
     * returns path to user configuration files directory (creates one if does not exist)
    */
    static std::string configDirectoryPath(const std::string &appName);
    /*
     * returns path to user cache files directory (creates one if does not exist)
    */
    static std::string cacheDirectoryPath(const std::string &appName);
};

#endif // CFGPATH_HPP
