#include "client/imageloader.hpp"

#include "common/servicelocator.hpp"

// SAIL is good
//#include <sail/sail.h>
//#include <sail/sail-manip/sail-manip.h>
#include <sail-c++/sail-c++.h>

#include <memory>
#include <cstring>

ImageData ImageLoader::loadImage(const std::string &path)
{
    DataResource res = ServiceLocator::getResourceManager().getResource(path);
    ImageData imgData = ImageLoader::loadImageMemory(static_pointer_cast<void>(res.data).get(), res.size);
    return imgData;
}

ImageData ImageLoader::loadImageMemory(const void *data, size_t size)
{
    sail::image srcImage = sail::image_input::load(data, size);
    if(!srcImage.is_valid())
        throw std::runtime_error("failed to load image");

    if(srcImage.convert(SAIL_PIXEL_FORMAT_BPP32_RGBA) != SAIL_OK)
        throw std::runtime_error("failed to convert image");

    ImageData res{};
    res.width = srcImage.width();
    res.height = srcImage.height();
    res.size = res.height * srcImage.bytes_per_line();
    res.data = std::make_shared<unsigned char[]>(res.size);
    const uint32_t bpl = srcImage.bytes_per_line();
    for(uint32_t i=0; i < res.height; i++) // flip vertically
        memcpy(res.data.get()+bpl*(res.height-i-1), srcImage.pixels()+bpl*i, bpl);
    return res;
}

ImageData ImageLoader::loadImageResource(const DataResource &res)
{
    return ImageLoader::loadImageMemory(res.data.get(), res.size);
}
