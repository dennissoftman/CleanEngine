#include <spdlog/spdlog.h>

#include "client/gamerendererdiligent.hpp"

using namespace Diligent;

// TEMPORARY ------------------------------------------------------------------
static const char* VSSource = R"(
struct PSInput 
{ 
    float4 Pos   : SV_POSITION; 
    float3 Color : COLOR; 
};

void main(in  uint    VertId : SV_VertexID,
          out PSInput PSIn) 
{
    float4 Pos[3];
    Pos[0] = float4(-0.5, -0.5, 0.0, 1.0);
    Pos[1] = float4( 0.0, +0.5, 0.0, 1.0);
    Pos[2] = float4(+0.5, -0.5, 0.0, 1.0);

    float3 Col[3];
    Col[0] = float3(1.0, 0.0, 0.0); // red
    Col[1] = float3(0.0, 1.0, 0.0); // green
    Col[2] = float3(0.0, 0.0, 1.0); // blue

    PSIn.Pos   = Pos[VertId];
    PSIn.Color = Col[VertId];
}
)";

// Pixel shader simply outputs interpolated vertex color
static const char* PSSource = R"(
struct PSInput 
{ 
    float4 Pos   : SV_POSITION; 
    float3 Color : COLOR; 
};

struct PSOutput
{ 
    float4 Color : SV_TARGET; 
};

void main(in  PSInput  PSIn,
          out PSOutput PSOut)
{
    PSOut.Color = float4(PSIn.Color.rgb, 1.0);
}
)";
// ============================================================================

Renderer* Renderer::create()
{
    return new GameRendererDiligent();
}

GameRendererDiligent::GameRendererDiligent()
{

}

static DiligentEngineRendererData InitializeDiligentEngine(SwapChainDesc SCDesc, RENDER_DEVICE_TYPE deviceType, HWND NativeWindowHandle)
{
    RefCntAutoPtr<IRenderDevice>  pDevice;
    RefCntAutoPtr<IDeviceContext> pImmediateContext;
    RefCntAutoPtr<ISwapChain>     pSwapChain;
    switch (deviceType)
    {
        case RENDER_DEVICE_TYPE_D3D11:
        {
            EngineD3D11CreateInfo EngineCI;
#    if ENGINE_DLL
            // Load the dll and import GetEngineFactoryD3D11() function
            auto* GetEngineFactoryD3D11 = LoadGraphicsEngineD3D11();
#    endif
            auto* pFactoryD3D11 = GetEngineFactoryD3D11();
            pFactoryD3D11->CreateDeviceAndContextsD3D11(EngineCI, &pDevice, &pImmediateContext);
            Win32NativeWindow Window{NativeWindowHandle};
            pFactoryD3D11->CreateSwapChainD3D11(pDevice, pImmediateContext, SCDesc,
                                                FullScreenModeDesc{}, Window, &pSwapChain);
        }
        break;

        case RENDER_DEVICE_TYPE_D3D12:
        {
#    if ENGINE_DLL
            // Load the dll and import GetEngineFactoryD3D12() function
            auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
#    endif
            EngineD3D12CreateInfo EngineCI;

            auto* pFactoryD3D12 = GetEngineFactoryD3D12();
            pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &pDevice, &pImmediateContext);
            Win32NativeWindow Window{NativeWindowHandle};
            pFactoryD3D12->CreateSwapChainD3D12(pDevice, pImmediateContext, SCDesc,
                                                FullScreenModeDesc{}, Window, &pSwapChain);
        }
        break;

    case RENDER_DEVICE_TYPE_GL:
    {
#if EXPLICITLY_LOAD_ENGINE_GL_DLL
        // Load the dll and import GetEngineFactoryOpenGL() function
        auto GetEngineFactoryOpenGL = LoadGraphicsEngineOpenGL();
#endif
        auto* pFactoryOpenGL = GetEngineFactoryOpenGL();

        EngineGLCreateInfo EngineCI;
        EngineCI.Window.hWnd = NativeWindowHandle;

        pFactoryOpenGL->CreateDeviceAndSwapChainGL(EngineCI, &pDevice, &pImmediateContext,
                                                   SCDesc, &pSwapChain);
    }
    break;

    case RENDER_DEVICE_TYPE_VULKAN:
    {
#    if EXPLICITLY_LOAD_ENGINE_VK_DLL
        // Load the dll and import GetEngineFactoryVk() function
        auto GetEngineFactoryVk = LoadGraphicsEngineVk();
#    endif
        EngineVkCreateInfo EngineCI;

        auto* pFactoryVk = GetEngineFactoryVk();
        pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &pDevice, &pImmediateContext);
        Win32NativeWindow Window{NativeWindowHandle};
        pFactoryVk->CreateSwapChainVk(pDevice, pImmediateContext, SCDesc, Window, &pSwapChain);
    }
    break;

    default:
        spdlog::error("Unknown graphical device type");
    }

    return DiligentEngineRendererData{pDevice, pImmediateContext, pSwapChain};
}

void GameRendererDiligent::init(const VideoMode &mode)
{
    m_size = glm::ivec2(mode.width, mode.height);

    SwapChainDesc SCDesc{};
    SCDesc.Width = mode.width;
    SCDesc.Height = mode.height;

    auto rendererData = InitializeDiligentEngine(SCDesc, RENDER_DEVICE_TYPE_D3D11, static_cast<HWND>(mode.osdata));
    m_pDevice = rendererData.m_pDevice;
    m_pImmediateContext = rendererData.m_pImmediateContext;
    m_pSwapChain = rendererData.m_pSwapChain;

    // TEMPORARY
    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSOCreateInfo.PSODesc.Name = "Simple triangle PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial will render to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets             = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0]                = m_pSwapChain->GetDesc().ColorBufferFormat;
    // Use the depth buffer format from the swap chain
    PSOCreateInfo.GraphicsPipeline.DSVFormat                    = m_pSwapChain->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // No back face culling for this tutorial
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_NONE;
    // Disable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;
    // Create a vertex shader
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "Triangle vertex shader";
        ShaderCI.Source          = VSSource;
        m_pDevice->CreateShader(ShaderCI, &pVS);
    }

    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "Triangle pixel shader";
        ShaderCI.Source          = PSSource;
        m_pDevice->CreateShader(ShaderCI, &pPS);
    }

    // Finally, create the pipeline state
    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;
    m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);
    // =========
}

void GameRendererDiligent::draw()
{
    // Clear the back buffer
    const float ClearColor[] = {0.350f, 0.350f, 0.350f, 1.0f};
    // Let the engine perform required state transitions
    auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
    auto* pDSV = m_pSwapChain->GetDepthBufferDSV();
    m_pImmediateContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    // Set pipeline state in the immediate context
    m_pImmediateContext->SetPipelineState(m_pPSO);

    // m_pImmediateContext->CommitShaderResources(nullptr, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    DrawAttribs drawAttrs;
    drawAttrs.NumVertices = 3;  // We will render 3 vertices
    m_pImmediateContext->Draw(drawAttrs);

    // Present
    if (!m_pSwapChain) {
        return;
    }
    m_pSwapChain->Present();
}

const glm::ivec2& GameRendererDiligent::getSize() const
{
    return m_size;
}

void GameRendererDiligent::resize(const glm::ivec2 &size)
{
    
    m_size = size;
    // resize graphics
    m_pSwapChain->Resize(m_size.x, m_size.y);
}

void GameRendererDiligent::updateCameraData(Camera3D &cam)
{

}

void GameRendererDiligent::updateLightCount(uint32_t count)
{

}

void GameRendererDiligent::updateLightPosition(const glm::vec4 &pos, uint32_t id)
{

}

void GameRendererDiligent::updateLightColor(const glm::vec4 &color, uint32_t id)
{

}

std::string GameRendererDiligent::getType() const
{
    return "dg";
}