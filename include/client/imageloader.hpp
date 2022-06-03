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
    size_t size; // image size
    size_t width, height;
    ImageFormat format;
};

class ImageLoader
{
public:
    static ImageData loadImage(const std::string &path);
    static ImageData loadImageMemory(const void *data, size_t size);
};

#endif // IMAGELOADER_HPP
