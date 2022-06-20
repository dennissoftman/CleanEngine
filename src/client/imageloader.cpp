#include "client/imageloader.hpp"

#include "common/servicelocator.hpp"

// stb_image is pretty good
#define STB_IMAGE_IMPLEMENTATION
// png, jpeg, bmp, tga, tiff support
#include <stb_image.h>
// dds support
#include <dds/dds.h>

#include <memory>
#include <cstring>

static const char *MODULE_NAME = "ImageLoader";

static const unsigned char dds_magic[] = {0x44, 0x44, 0x53, 0x20};

ImageData ImageLoader::loadImage(const std::string &path)
{
    DataResource res = ServiceLocator::getResourceManager().getResource(path);
    ImageData imgData = ImageLoader::loadImageMemory(static_pointer_cast<void>(res.data).get(), res.size);
    return imgData;
}

ImageData ImageLoader::loadImageMemory(const void *data, size_t size)
{
    ImageData res{};
    if(memcmp(data, dds_magic, sizeof(dds_magic)) == 0)
    {
        dds_image_t img = dds_load_from_memory(static_cast<const char*>(data), size);
        if(img == NULL)
            throw std::runtime_error("failed to load dds image");

        res.width = img->header.width;
        res.height = img->header.height;
        res.format = ImageFormat::eRGBA;
        res.size = res.width * res.height * 4;
        res.data = std::make_shared<unsigned char[]>(res.size);
        memcpy(res.data.get(), img->pixels, res.size);
        dds_image_free(img);
    }
    else
    {
        int width, height, bpp;
        stbi_set_flip_vertically_on_load(1);
        stbi_uc *img = stbi_load_from_memory(static_cast<const unsigned char*>(data), size, &width, &height, &bpp, STBI_rgb_alpha);
        if(img == NULL)
            throw std::runtime_error("failed to load image");

        res.width = width;
        res.height = height;
        res.format = ImageFormat::eRGBA;
        res.size = width * height * 4; // RGBA
        res.data = std::make_shared<unsigned char[]>(res.size);
        memcpy(res.data.get(), img, res.size);
        stbi_image_free(img);
    }
    return res;
}

ImageData ImageLoader::loadImageResource(const DataResource &res)
{
    return ImageLoader::loadImageMemory(res.data.get(), res.size);
}
