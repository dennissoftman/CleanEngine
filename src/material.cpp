#include "material.hpp"

#ifdef RENDERER_OPENGL
#include "glmaterial.hpp"

Material *Material::createMaterial()
{
    return new GLMaterial();
}

#elif RENDERER_VULKAN
#include "vkmaterial.hpp"

Material *Material::createMaterial()
{
    return new VkMaterial();
}
#endif
