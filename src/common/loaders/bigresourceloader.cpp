#include <fstream>
#include <filesystem>
#include <boost/algorithm/string.hpp>
#include <stdexcept>
#include <spdlog/spdlog.h>

#include "common/loaders/bigresourceloader.hpp"

BigFileEntry::BigFileEntry(std::ifstream& file)
{
    uint32_t val;
    file.read(reinterpret_cast<char*>(&val), sizeof(m_dataOffset));
    m_dataOffset = boost::endian::big_to_native(val);
    file.read(reinterpret_cast<char*>(&val), sizeof(m_dataSize));
    m_dataSize = boost::endian::big_to_native(val);
    std::getline(file, m_filename, '\0');
}

uint32_t BigFileEntry::dataOffset() const
{
    return m_dataOffset;
}

uint32_t BigFileEntry::dataSize() const
{
    return m_dataSize;
}

const std::string& BigFileEntry::filename() const
{
    return m_filename;
}

BigFileStructure::BigFileStructure(std::ifstream& file)
{
    file.read(m_magic, 4);
    if(strncmp(m_magic, BIG_ARCH_MAGIC, 4))
    {
        throw std::runtime_error("Invalid big archive");
        return;
    }
    uint32_t val;
    file.read(reinterpret_cast<char*>(&val), sizeof(uint32_t));
    m_archiveSize = boost::endian::little_to_native(val);
    file.read(reinterpret_cast<char*>(&val), sizeof(uint32_t));
    m_entryCount = boost::endian::big_to_native(val);
    file.read(reinterpret_cast<char*>(&val), sizeof(uint32_t));
    m_dataOffset = boost::endian::big_to_native(val);
}

uint32_t BigFileStructure::archiveSize() const
{
    return m_archiveSize;
}

uint32_t BigFileStructure::entryCount() const
{
    return m_entryCount;
}

uint32_t BigFileStructure::dataOffset() const
{
    return m_dataOffset;
}

BigResourceLoader::BigResourceLoader(const std::vector<std::string>& bigFiles)
    : m_bigFiles(bigFiles)
{

}

void BigResourceLoader::addBigFile(const std::string& path)
{
    m_bigFiles.emplace_back(path);
}

bool BigResourceLoader::contains(const std::string& path) const
{
    for(auto& bigFile : m_bigFiles)
    {
        std::ifstream file(bigFile, std::ios::binary);
        if(file)
        {
            BigFileStructure header(file);

            for(uint32_t i = 0; i < header.entryCount(); ++i)
            {
                BigFileEntry entry(file);

                auto p0 = std::filesystem::path(entry.filename()).make_preferred();
                auto p1 = std::filesystem::path(path).make_preferred();
                if(boost::iequals(p0.string(), p1.string()))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

std::shared_ptr<DataResource> BigResourceLoader::get(const std::string& path, bool caching) const
{
    for(auto& bigFile : m_bigFiles)
    {
        std::ifstream file(bigFile, std::ios::binary);
        if(file)
        {
            BigFileStructure header(file);

            for(uint32_t i = 0; i < header.entryCount(); ++i)
            {
                BigFileEntry entry(file);

                auto p0 = std::filesystem::path(entry.filename()).make_preferred();
                auto p1 = std::filesystem::path(path).make_preferred();
                if(boost::iequals(p0.string(), p1.string()))
                {
                    auto resource = std::make_shared<DataResource>();
                    resource->data.resize(entry.dataSize());
                    file.seekg(entry.dataOffset(), std::ios::beg);
                    file.read(reinterpret_cast<char*>(resource->data.data()), entry.dataSize());
                    return resource;
                }
            }
        }
    }
    throw std::runtime_error(fmt::format("File '{}' not found", path));
}

const char* BigResourceLoader::getType() const
{
    return "BigResourceLoader";
}