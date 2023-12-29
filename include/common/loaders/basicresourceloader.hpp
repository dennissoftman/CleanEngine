#ifndef BASIC_RESOURCE_LOADER_HPP
#define BASIC_RESOURCE_LOADER_HPP

#include <string>
#include <memory>
#include <vector>

class DataResource
{
public:
    std::vector<char> data;
};

class BasicResourceLoader {
public:
    virtual ~BasicResourceLoader() = default;

    virtual bool contains(const std::string& path) const = 0;  // check if resource exists
    virtual std::shared_ptr<DataResource> get(const std::string& path, bool caching=false) const = 0;  // get resource

    virtual const char* getType() const = 0;  // get type of resource loader
};

#endif