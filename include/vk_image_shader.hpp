#ifndef VK_IMAGE_SHADER_HPP
#define VK_IMAGE_SHADER_HPP

#include <cstddef>

class vk_image_shader
{
public:
    static const char *vert_data();
    static size_t vert_size();

    static const char *frag_data();
    static size_t frag_size();
};


#endif // VK_IMAGE_SHADER_HPP
