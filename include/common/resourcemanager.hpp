#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <vector>
#include <map>
#include <string>
#include <memory>

#include <minizip/unzip.h>

// yes, redefining tar structure, so what will you do?
struct alignas(512) ustar_header_s
{
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char _[12]; // padding
};
//

struct DataResource
{
    std::shared_ptr<char[]> data;
    size_t size;
};

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    void init();
    void terminate();

    DataResource getResource(const std:: string &path, bool enableCaching=false);

    enum TarFileType
    {
        eRegType='0',
        eARegType='\0',
        aLnkType='1',
        eSymType='2',
        eChrType='3',
        eBlkType='4',
        eDirType='5',
        eFifoType='6',
        eContType='7'
    };
    enum TarProperty
    {
        eBlockSize=512,
    };
private:
    std::map<std::string, DataResource> m_cachedResources;

    std::map<std::string, unzFile> m_resourceArchives;
};

#endif // RESOURCEMANAGER_HPP
