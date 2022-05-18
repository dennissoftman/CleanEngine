#include "material.hpp"

#ifdef RENDERER_OPENGL
#include "glmaterial.hpp"
#include "servicelocator.hpp"

Material *Material::createMaterial()
{
    OpenGLRenderer *glRenderer = dynamic_cast<OpenGLRenderer*>(&ServiceLocator::getRenderer());
    return new GLMaterial(glRenderer);
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
