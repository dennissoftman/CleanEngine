#ifndef IMAGELOADER_HPP
#define IMAGELOADER_HPP

#include <string>
#include <memory>

enum class ImageFormat
{
    eBW,
    eGrayscale,
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

class DataResource;

class ImageLoader
{
public:
    static ImageData loadImage(const std::string &path);
    static ImageData loadImageMemory(const void *data, size_t size);
    static ImageData loadImageResource(const DataResource &res);
};

#endif // IMAGELOADER_HPP
