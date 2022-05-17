#include "material.hpp"

#ifdef RENDERER_OPENGL
#include "glmaterial.hpp"

Material *Material::createMaterial()
{
    return new GLMaterial();
}

#elif RENDERER_VULKAN
#include "vkmaterial.hpp"
#include "servicelocator.hpp"

Material *Material::createMaterial()
{
    VulkanRenderer *vkRenderer = dynamic_cast<VulkanRenderer*>(&ServiceLocator::getRenderer());
    return new VkMaterial(vkRenderer);
}
#endif
