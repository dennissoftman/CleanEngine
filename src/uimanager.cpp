#ifdef RENDERER_VULKAN
#ifdef UI_IMGUI
#include "imguivkuimanager.hpp"

UIManager *UIManager::create()
{
    return new ImguiVkUIManager();
}
#else
UIManager *UIManager::create()
{
    return nullptr;
}
#endif
#else
#error Unsupported graphics library
#endif

