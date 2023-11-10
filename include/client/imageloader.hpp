#ifndef IMAGELOADER_HPP
#define IMAGELOADER_HPP

#include <string>
#include <memory>

enum class ImageFormat
{
    eUINT16,
    eRGB,
    eRGBA
};

struct ImageData
{
    std::shared_ptr<unsigned char[]> data;
    size_t size=0; // image size
    size_t width, height;
    ImageFormat format;
};

struct DataResource;

class ImageLoader
{
public:
    static ImageData loadImage(const std::string &path);
    static ImageData loadImageMemory(const void *data, size_t size);
    static ImageData loadImageResource(const DataResource &res);
};

#endif // IMAGELOADER_HPP
