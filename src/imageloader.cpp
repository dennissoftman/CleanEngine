#include "imageloader.hpp"

#include "servicelocator.hpp"

#include <sail/sail.h>
#include <sail/sail-manip/sail-manip.h>

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
    sail_image *srcImage;

    if(sail_load_image_from_memory(data, size, &srcImage) != SAIL_OK)
        throw std::runtime_error("Failed to load image");

    if(sail_flip_vertically(srcImage) != SAIL_OK)
        throw std::runtime_error("Failed to flip image");

    sail_image *dstImage;
    if(sail_convert_image(srcImage, SAIL_PIXEL_FORMAT_BPP32_RGBA, &dstImage) != SAIL_OK)
    {
        sail_destroy_image(srcImage);
        throw std::runtime_error("Failed to convert image");
    }
    sail_destroy_image(srcImage);

    ImageData res{};
    res.width = dstImage->width;
    res.height = dstImage->height;
    res.size = res.height * dstImage->bytes_per_line;
    res.data = std::make_shared<unsigned char[]>(res.size);
    memcpy(res.data.get(), dstImage->pixels, res.size);
    sail_destroy_image(dstImage);

    return res;
}
