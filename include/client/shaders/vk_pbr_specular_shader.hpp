#ifndef VK_PBR_SPECULAR_SHADER_HPP
#define VK_PBR_SPECULAR_SHADER_HPP

#include <cstddef>

class vk_pbr_specular_shader
{
public:
    static const char *vert_data();
    static size_t vert_size();

    static const char *frag_data();
    static size_t frag_size();
};


#endif // VK_PBR_SPECULAR_SHADER_HPP
