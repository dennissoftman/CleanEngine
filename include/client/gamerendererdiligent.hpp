#ifndef GAMERENDERERDILIGENT_HPP
#define GAMERENDERERDILIGENT_HPP

#ifdef _WIN32
    #define PLATFORM_WIN32 (1)
    #include <windows.h>
#elif __linux__
    #define PLATFORM_LINUX (1)
#endif

#include "Graphics/GraphicsEngine/interface/EngineFactory.h"
#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/RenderDevice.h"
#include "Graphics/GraphicsEngine/interface/DeviceContext.h"
#include "Graphics/GraphicsEngine/interface/SwapChain.h"
#include "Common/interface/BasicMath.hpp"
#include "Primitives/interface/FlagEnum.h"

#include "Graphics/GraphicsEngineD3D11/interface/EngineFactoryD3D11.h"
#include "Graphics/GraphicsEngineD3D12/interface/EngineFactoryD3D12.h"
#include "Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h"
#include "Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h"

#include "client/renderer.hpp"

struct DiligentEngineRendererData
{
    Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
    Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_pSwapChain;

    DiligentEngineRendererData()
        : m_pDevice(nullptr), m_pImmediateContext(nullptr), m_pSwapChain(nullptr)
    {

    }

    DiligentEngineRendererData(Diligent::RefCntAutoPtr<Diligent::IRenderDevice> pDevice,
                               Diligent::RefCntAutoPtr<Diligent::IDeviceContext> pImmediateContext,
                               Diligent::RefCntAutoPtr<Diligent::ISwapChain> pSwapChain)
    {
        m_pDevice = pDevice;
        m_pImmediateContext = pImmediateContext;
        m_pSwapChain = pSwapChain;
    }
};

class GameRendererDiligent : public Renderer
{
public:
    GameRendererDiligent();

    void init(const VideoMode &mode);
    void draw();
    const glm::ivec2& getSize() const;
    void resize(const glm::ivec2 &size);
    void updateCameraData(Camera3D &cam);
    void updateLightCount(uint32_t count);
    void updateLightPosition(const glm::vec4 &pos, uint32_t id);
    void updateLightColor(const glm::vec4 &color, uint32_t id);
    std::string getType() const;

private:
    glm::ivec2 m_size;

    Diligent::RefCntAutoPtr<Diligent::IRenderDevice>  m_pDevice;
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
    Diligent::RefCntAutoPtr<Diligent::ISwapChain>     m_pSwapChain;
    // TEMPORARY
    Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pPSO;
};

#endif // GAMERENDERERDILIGENT_HPP
