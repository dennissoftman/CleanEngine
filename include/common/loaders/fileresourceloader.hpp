#ifndef FILE_RESOURCE_LOADER_HPP
#define FILE_RESOURCE_LOADER_HPP

#include "common/loaders/basicresourceloader.hpp"

class FileResourceLoader : public BasicResourceLoader {
public:
    FileResourceLoader(const std::vector<std::string>& paths);
    ~FileResourceLoader() = default;

    void addPath(const std::string& path);

    bool contains(const std::string& path) const override;
    std::shared_ptr<DataResource> get(const std::string& path, bool caching=false) const override;

    const char* getType() const override;

private:
    std::vector<std::string> m_paths;
};

#endif