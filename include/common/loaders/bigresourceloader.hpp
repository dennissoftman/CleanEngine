#ifndef BIG_RESOURCE_LOADER_HPP
#define BIG_RESOURCE_LOADER_HPP

#include <boost/endian.hpp>

#include "common/loaders/basicresourceloader.hpp"

#define BIG_ARCH_MAGIC     "BIGF"
#define BIG_ARCH_END_MARK  (0x4C323331)

class BigFileEntry {
public:
    BigFileEntry(std::ifstream& file);

    uint32_t dataOffset() const;
    uint32_t dataSize() const;
    const std::string& filename() const;

private:
    uint32_t m_dataOffset;  // big endian
    uint32_t m_dataSize;    // big endian
    std::string m_filename;
};

class BigFileStructure {
public:
    BigFileStructure(std::ifstream& file);

    uint32_t archiveSize() const;
    uint32_t entryCount() const;
    uint32_t dataOffset() const;
private:
    char m_magic[4];        // should be 'BIGF'
    uint32_t m_archiveSize; // little endian
    uint32_t m_entryCount;  // big endian
    uint32_t m_dataOffset;  // big endian
};

class BigResourceLoader : public BasicResourceLoader {
public:
    BigResourceLoader(const std::vector<std::string>& bigFiles);

    void addBigFile(const std::string& path);

    bool contains(const std::string& path) const override;
    std::shared_ptr<DataResource> get(const std::string& path, bool caching=false) const override;

    const char* getType() const override;
private:
    std::vector<std::string> m_bigFiles;
};

#endif