#include <spdlog/spdlog.h>
#include <filesystem>
#include <boost/algorithm/string.hpp>
#include <algorithm>

#include "client/gamerendererdiligent.hpp"
#include "common/servicelocator.hpp"
#include "Graphics/GraphicsTools/interface/GraphicsUtilities.h"
#include "Graphics/GraphicsEngine/interface/GraphicsTypesX.hpp"
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"
#include "TextureLoader/interface/TextureUtilities.h"
#include "Common/interface/DataBlobImpl.hpp"

#include <SOIL2/SOIL2.h>

using namespace Diligent;

Renderer* Renderer::create()
{
    return new GameRendererDiligent();
}

GameRendererDiligent::GameRendererDiligent()
    : m_elapsedTime(0.0)
{

}

DiligentEngineRendererData GameRendererDiligent::InitializeDiligentEngine(SwapChainDesc SCDesc, RENDER_DEVICE_TYPE deviceType)
{
    RefCntAutoPtr<IRenderDevice>  pDevice;
    RefCntAutoPtr<IDeviceContext> pImmediateContext;
    RefCntAutoPtr<ISwapChain>     pSwapChain;
    switch (deviceType)
    {
        case RENDER_DEVICE_TYPE_D3D12:
        {
#    if ENGINE_DLL
            // Load the dll and import GetEngineFactoryD3D12() function
            auto GetEngineFactoryD3D12 = LoadGraphicsEngineD3D12();
#    endif
            EngineD3D12CreateInfo EngineCI;

            auto* pFactoryD3D12 = GetEngineFactoryD3D12();
            m_pEngineFactory = pFactoryD3D12;

            pFactoryD3D12->CreateDeviceAndContextsD3D12(EngineCI, &pDevice, &pImmediateContext);
            Win32NativeWindow Window{m_videoMode.osdata()};
            auto fsModeDesc = FullScreenModeDesc{};
            fsModeDesc.Fullscreen = m_videoMode.fullscreen();
            pFactoryD3D12->CreateSwapChainD3D12(pDevice, pImmediateContext, SCDesc,
                                                fsModeDesc, Window, &pSwapChain);
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
        m_pEngineFactory = pFactoryVk;

        pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &pDevice, &pImmediateContext);
        Win32NativeWindow Window{m_videoMode.osdata()};
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
    m_videoMode = mode;
    m_size = glm::ivec2(m_videoMode.width(), m_videoMode.height());

    SwapChainDesc SCDesc{};
    SCDesc.Width = m_videoMode.width();
    SCDesc.Height = m_videoMode.height();
    m_syncInterval = m_videoMode.vsync() ? 1U : 0U;

    RENDER_DEVICE_TYPE deviceType;
    if(m_videoMode.renderingBackend() == "vk")
        deviceType = RENDER_DEVICE_TYPE_VULKAN;
    else if(m_videoMode.renderingBackend() == "dx12")
        deviceType = RENDER_DEVICE_TYPE_D3D12;
    
    m_ShadowMapSize = (Uint32)m_videoMode.shadowMapResolution();

    auto rendererData = InitializeDiligentEngine(SCDesc, deviceType);
    m_pDevice = rendererData.m_pDevice;
    m_pImmediateContext = rendererData.m_pImmediateContext;
    m_pSwapChain = rendererData.m_pSwapChain;

    // TEMPORARY
    std::vector<StateTransitionDesc> Barriers;
    // Create dynamic uniform buffer that will store our transformation matrices
    // Dynamic buffers can be frequently updated by the CPU
    const int MAX_MESH_TRANSFORMS = 100;
    CreateUniformBuffer(m_pDevice, sizeof(float4x4) * 2 + sizeof(float4) + sizeof(float4x4) * MAX_MESH_TRANSFORMS, "VS constants CB", &m_VSConstants);
    Barriers.emplace_back(m_VSConstants, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_CONSTANT_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);

    CreateTexturedPSO();

    auto absentTexture = GenerateEmptyTexture();
    m_defaultSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Texture")->Set(absentTexture->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
    // Transition the texture to shader resource state
    Barriers.emplace_back(absentTexture, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);

    CreateShadowMapVisPSO();
    CreateShadowMap();

    m_pImmediateContext->TransitionResourceStates(static_cast<Uint32>(Barriers.size()), Barriers.data());

    // TEMP
    CreateTextureMaterial("textures/shotgun.png", "shotgun");
}

void GameRendererDiligent::CreateTexturedPSO()
{
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    m_pEngineFactory->CreateDefaultShaderSourceStreamFactory("./data/shaders/", &pShaderSourceFactory);

    TexturedCube::CreatePSOInfo CubePsoCI;
    CubePsoCI.pDevice              = m_pDevice;
    CubePsoCI.RTVFormat            = m_pSwapChain->GetDesc().ColorBufferFormat;
    CubePsoCI.DSVFormat            = m_pSwapChain->GetDesc().DepthBufferFormat;
    CubePsoCI.pShaderSourceFactory = pShaderSourceFactory;
    CubePsoCI.VSFilePath           = "texture.vsh";
    CubePsoCI.PSFilePath           = "texture.psh";
    CubePsoCI.Components           = TexturedCube::VERTEX_COMPONENT_FLAG_POS_NORM_UV;

    m_pCubePSO = TexturedCube::CreatePipelineState(CubePsoCI, false);

    // Since we did not explicitly specify the type for 'Constants' variable, default
    // type (SHADER_RESOURCE_VARIABLE_TYPE_STATIC) will be used. Static variables never
    // change and are bound directly through the pipeline state object.
    m_pCubePSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_VSConstants);

    m_pCubePSO->CreateShaderResourceBinding(&m_defaultSRB, true); 

    // Create shadow pass PSO
    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    PSOCreateInfo.PSODesc.Name = "Shadow PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // Shadow pass doesn't use any render target outputs
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets             = 0;
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0]                = TEX_FORMAT_UNKNOWN;
    // The DSV format is the shadow map format
    PSOCreateInfo.GraphicsPipeline.DSVFormat                    = m_ShadowMapFormat;
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Cull back faces
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.FrontCounterClockwise = True;
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;
    // Enable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;
    // Create shadow vertex shader
    RefCntAutoPtr<IShader> pShadowVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "Shadow VS";
        ShaderCI.FilePath        = "shadow.vsh";
        m_pDevice->CreateShader(ShaderCI, &pShadowVS);
    }
    PSOCreateInfo.pVS = pShadowVS;

    // We don't use pixel shader as we are only interested in populating the depth buffer
    PSOCreateInfo.pPS = nullptr;

    // clang-format off
    // Define vertex shader input layout
    LayoutElement LayoutElems[] =
    {
        // Attribute 0 - vertex position
        LayoutElement{0, 0, 3, VT_FLOAT32, False},
        // Attribute 1 - normal
        LayoutElement{2, 0, 3, VT_FLOAT32, False},
        // Attribute 2 - texture coordinates
        LayoutElement{1, 0, 2, VT_FLOAT32, False},
    };
    // clang-format on

    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = LayoutElems;
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements    = _countof(LayoutElems);

    PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    if (m_pDevice->GetDeviceInfo().Features.DepthClamp)
    {
        // Disable depth clipping to render objects that are closer than near
        // clipping plane. This is not required for this tutorial, but real applications
        // will most likely want to do this.
        PSOCreateInfo.GraphicsPipeline.RasterizerDesc.DepthClipEnable = False;
    }

    m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pCubeShadowPSO);
    m_pCubeShadowPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(m_VSConstants);
    m_pCubeShadowPSO->CreateShaderResourceBinding(&m_CubeShadowSRB, true);
}

void GameRendererDiligent::CreateShadowMapVisPSO()
{
    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    m_pEngineFactory->CreateDefaultShaderSourceStreamFactory("./data/shaders/", &pShaderSourceFactory);
    
    GraphicsPipelineStateCreateInfo PSOCreateInfo;

    PSOCreateInfo.PSODesc.Name = "Shadow Map Vis PSO";

    // This is a graphics pipeline
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // clang-format off
    // This tutorial renders to a single render target
    PSOCreateInfo.GraphicsPipeline.NumRenderTargets             = 1;
    // Set render target format which is the format of the swap chain's color buffer
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0]                = m_pSwapChain->GetDesc().ColorBufferFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    PSOCreateInfo.GraphicsPipeline.DSVFormat                    = m_pSwapChain->GetDesc().DepthBufferFormat;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    // No culing
    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_NONE;
    // Disable depth testing
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;
    // clang-format on

    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;
    // Create shadow map visualization vertex shader
    RefCntAutoPtr<IShader> pShadowMapVisVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "Shadow Map Vis VS";
        ShaderCI.FilePath        = "shadow_map_vis.vsh";
        m_pDevice->CreateShader(ShaderCI, &pShadowMapVisVS);
    }

    // Create shadow map visualization pixel shader
    RefCntAutoPtr<IShader> pShadowMapVisPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "Shadow Map Vis PS";
        ShaderCI.FilePath        = "shadow_map_vis.psh";
        m_pDevice->CreateShader(ShaderCI, &pShadowMapVisPS);
    }

    PSOCreateInfo.pVS = pShadowMapVisVS;
    PSOCreateInfo.pPS = pShadowMapVisPS;

    // Define variable type that will be used by default
    PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE;

    // clang-format off
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR,
        TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
    };
    ImmutableSamplerDesc ImtblSamplers[] =
    {
        {SHADER_TYPE_PIXEL, "g_ShadowMap", SamLinearClampDesc}
    };
    // clang-format on
    PSOCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers    = ImtblSamplers;
    PSOCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);

    m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pShadowMapVisPSO);
}

void GameRendererDiligent::CreateShadowMap()
{
    TextureDesc SMDesc;
    SMDesc.Name      = "Shadow map";
    SMDesc.Type      = RESOURCE_DIM_TEX_2D;
    SMDesc.Width     = m_ShadowMapSize;
    SMDesc.Height    = m_ShadowMapSize;
    SMDesc.Format    = m_ShadowMapFormat;
    SMDesc.BindFlags = BIND_SHADER_RESOURCE | BIND_DEPTH_STENCIL;
    RefCntAutoPtr<ITexture> ShadowMap;
    m_pDevice->CreateTexture(SMDesc, nullptr, &ShadowMap);
    m_ShadowMapSRV = ShadowMap->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE);
    m_ShadowMapDSV = ShadowMap->GetDefaultView(TEXTURE_VIEW_DEPTH_STENCIL);

    m_ShadowMapVisSRB.Release();
    m_pShadowMapVisPSO->CreateShaderResourceBinding(&m_ShadowMapVisSRB, true);
    m_ShadowMapVisSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_ShadowMap")->Set(m_ShadowMapSRV);
}

float4x4 GameRendererDiligent::GetSurfacePretransformMatrix(const float3& f3CameraViewAxis) const
{
    const auto& SCDesc = m_pSwapChain->GetDesc();
    switch (SCDesc.PreTransform)
    {
        case SURFACE_TRANSFORM_ROTATE_90:
            // The image content is rotated 90 degrees clockwise.
            return float4x4::RotationArbitrary(f3CameraViewAxis, -PI_F / 2.f);

        case SURFACE_TRANSFORM_ROTATE_180:
            // The image content is rotated 180 degrees clockwise.
            return float4x4::RotationArbitrary(f3CameraViewAxis, -PI_F);

        case SURFACE_TRANSFORM_ROTATE_270:
            // The image content is rotated 270 degrees clockwise.
            return float4x4::RotationArbitrary(f3CameraViewAxis, -PI_F * 3.f / 2.f);

        case SURFACE_TRANSFORM_OPTIMAL:
            UNEXPECTED("SURFACE_TRANSFORM_OPTIMAL is only valid as parameter during swap chain initialization.");
            return float4x4::Identity();

        case SURFACE_TRANSFORM_HORIZONTAL_MIRROR:
        case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90:
        case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180:
        case SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270:
            UNEXPECTED("Mirror transforms are not supported");
            return float4x4::Identity();

        default:
            return float4x4::Identity();
    }
}

void GameRendererDiligent::update(double dt)
{

}

void GameRendererDiligent::RenderShadowMap()
{
    float3 f3LightSpaceX, f3LightSpaceY, f3LightSpaceZ;
    f3LightSpaceZ = normalize(m_LightDirection);

    auto min_cmp = std::min(std::min(std::abs(m_LightDirection.x), std::abs(m_LightDirection.y)), std::abs(m_LightDirection.z));
    if (min_cmp == std::abs(m_LightDirection.x))
        f3LightSpaceX = float3(1, 0, 0);
    else if (min_cmp == std::abs(m_LightDirection.y))
        f3LightSpaceX = float3(0, 1, 0);
    else
        f3LightSpaceX = float3(0, 0, 1);

    f3LightSpaceY = cross(f3LightSpaceZ, f3LightSpaceX);
    f3LightSpaceX = cross(f3LightSpaceY, f3LightSpaceZ);
    f3LightSpaceX = normalize(f3LightSpaceX);
    f3LightSpaceY = normalize(f3LightSpaceY);

    float4x4 WorldToLightViewSpaceMatr = float4x4::ViewFromBasis(f3LightSpaceX, f3LightSpaceY, f3LightSpaceZ);

    // For this tutorial we know that the scene center is at (0,0,0).
    // Real applications will want to compute tight bounds

    float3 f3SceneCenter = float3(0, 0, 0);
    float  SceneRadius   = std::sqrt(3.f);
    float3 f3MinXYZ      = f3SceneCenter - float3(SceneRadius, SceneRadius, SceneRadius);
    float3 f3MaxXYZ      = f3SceneCenter + float3(SceneRadius, SceneRadius, SceneRadius * 5);
    float3 f3SceneExtent = f3MaxXYZ - f3MinXYZ;

    const auto& DevInfo = m_pDevice->GetDeviceInfo();
    const bool  IsGL    = DevInfo.IsGLDevice();
    float4      f4LightSpaceScale;
    f4LightSpaceScale.x = 2.f / f3SceneExtent.x;
    f4LightSpaceScale.y = 2.f / f3SceneExtent.y;
    f4LightSpaceScale.z = (IsGL ? 2.f : 1.f) / f3SceneExtent.z;
    // Apply bias to shift the extent to [-1,1]x[-1,1]x[0,1] for DX or to [-1,1]x[-1,1]x[-1,1] for GL
    // Find bias such that f3MinXYZ -> (-1,-1,0) for DX or (-1,-1,-1) for GL
    float4 f4LightSpaceScaledBias;
    f4LightSpaceScaledBias.x = -f3MinXYZ.x * f4LightSpaceScale.x - 1.f;
    f4LightSpaceScaledBias.y = -f3MinXYZ.y * f4LightSpaceScale.y - 1.f;
    f4LightSpaceScaledBias.z = -f3MinXYZ.z * f4LightSpaceScale.z + (IsGL ? -1.f : 0.f);

    float4x4 ScaleMatrix      = float4x4::Scale(f4LightSpaceScale.x, f4LightSpaceScale.y, f4LightSpaceScale.z);
    float4x4 ScaledBiasMatrix = float4x4::Translation(f4LightSpaceScaledBias.x, f4LightSpaceScaledBias.y, f4LightSpaceScaledBias.z);

    // Note: bias is applied after scaling!
    float4x4 ShadowProjMatr = ScaleMatrix * ScaledBiasMatrix;

    // Adjust the world to light space transformation matrix
    float4x4 WorldToLightProjSpaceMatr = WorldToLightViewSpaceMatr * ShadowProjMatr;

    const auto& NDCAttribs    = DevInfo.GetNDCAttribs();
    float4x4    ProjToUVScale = float4x4::Scale(0.5f, NDCAttribs.YtoVScale, NDCAttribs.ZtoDepthScale);
    float4x4    ProjToUVBias  = float4x4::Translation(0.5f, 0.5f, NDCAttribs.GetZtoDepthBias());

    m_WorldToShadowMapUVDepthMatr = WorldToLightProjSpaceMatr * ProjToUVScale * ProjToUVBias;

    std::for_each(m_queuedRenderObjects.begin(), m_queuedRenderObjects.end(), [this](const RenderData& renderData) { Render(renderData, false); } );
}

void GameRendererDiligent::Render(const RenderData& renderData, bool IsShadowPass)
{
    if(renderData.modelId >= m_models.size())
        return;

    const float4x4& modelMatrix = renderData.modelMatrix;

    struct ShaderConstants
    {
        float4x4 WorldViewProj;
        float4x4 NormalTranform;
        float4   LightDirection;
        float4x4 MeshTransforms[100];
    };
    MapHelper<ShaderConstants> CBConstants(m_pImmediateContext, m_VSConstants, MAP_WRITE, MAP_FLAG_DISCARD);
    
    // reset mesh transforms
    float4x4 identityMatrix = float4x4::Identity();
    for (int i = 0; i < 100; i++)
        memcpy(&(CBConstants->MeshTransforms[i]), identityMatrix.Data(), sizeof(float4x4));

    auto NormalMatrix  = modelMatrix.RemoveTranslation().Inverse();
    // We need to do inverse-transpose, but we also need to transpose the matrix
    // before writing it to the buffer
    CBConstants->WorldViewProj = (modelMatrix * m_viewMatrix * m_projectionMatrix).Transpose();
    CBConstants->NormalTranform = NormalMatrix;
    CBConstants->LightDirection = m_LightDirection;

    // Bind vertex buffer
    const auto& model = m_models[renderData.modelId];
    for(auto meshId = 0; meshId < model.meshCount; meshId++)
    {
        // Map the buffer and write current world-view-projection matrix
        if(meshId < renderData.meshMatrices.size()) {
            CBConstants->MeshTransforms[meshId] = renderData.meshMatrices.at(meshId);
        }
        //
        const auto& vertexBuffer = model.vertexBuffers[meshId];
        const auto& indexBuffer  = model.indexBuffers[meshId];
        const auto& indexCount   = model.indexCounts[meshId];

        IBuffer* pBuffs[] = {vertexBuffer};
        // Note that since resources have been explicitly transitioned to required states, we use RESOURCE_STATE_TRANSITION_MODE_VERIFY flag
        m_pImmediateContext->SetVertexBuffers(0, 1, pBuffs, nullptr, RESOURCE_STATE_TRANSITION_MODE_VERIFY, SET_VERTEX_BUFFERS_FLAG_RESET);
        m_pImmediateContext->SetIndexBuffer(indexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

        // Set pipeline state and commit resources
        if (IsShadowPass)
        {
            m_pImmediateContext->SetPipelineState(m_pCubeShadowPSO);
            m_pImmediateContext->CommitShaderResources(m_CubeShadowSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);
        }
        else
        {
            m_pImmediateContext->SetPipelineState(m_pCubePSO);
            // m_pImmediateContext->CommitShaderResources(m_materials[renderData.materialIds[meshId]], RESOURCE_STATE_TRANSITION_MODE_VERIFY);
            m_pImmediateContext->CommitShaderResources(m_materials[0], RESOURCE_STATE_TRANSITION_MODE_VERIFY);
        }

        DrawIndexedAttribs DrawAttrs(indexCount, VT_UINT32, DRAW_FLAG_VERIFY_ALL);
        m_pImmediateContext->DrawIndexed(DrawAttrs);
    }
}

void GameRendererDiligent::queueRender(size_t modelId, const std::vector<size_t>& materialIds, const glm::mat4& modelMatrix)
{
    float4x4 mx;
    memcpy(mx.Data(), &modelMatrix[0][0], sizeof(float) * 16);
    m_queuedRenderObjects.emplace_back(modelId, materialIds, mx, std::vector<float4x4>{});
}

void GameRendererDiligent::queueRender(size_t modelId, const std::vector<size_t>& materialIds, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& meshMatrices)
{
    float4x4 mdmx;
    memcpy(mdmx.Data(), &modelMatrix[0][0], sizeof(float) * 16);

    std::vector<float4x4> meshmxs;
    meshmxs.reserve(meshMatrices.size());
    for(const auto& mx : meshMatrices)
    {
        float4x4 m;
        memcpy(m.Data(), &mx[0][0], sizeof(float) * 16);
        meshmxs.emplace_back(m);
    }
    m_queuedRenderObjects.emplace_back(modelId, materialIds, mdmx, meshmxs);
}

void GameRendererDiligent::RenderShadowMapVis()
{
    m_pImmediateContext->SetPipelineState(m_pShadowMapVisPSO);
    m_pImmediateContext->CommitShaderResources(m_ShadowMapVisSRB, RESOURCE_STATE_TRANSITION_MODE_VERIFY);

    DrawAttribs DrawAttrs(4, DRAW_FLAG_VERIFY_ALL);
    m_pImmediateContext->Draw(DrawAttrs);
}

void GameRendererDiligent::draw()
{
    // // Render shadow map
    // m_pImmediateContext->SetRenderTargets(0, nullptr, m_ShadowMapDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    // m_pImmediateContext->ClearDepthStencil(m_ShadowMapDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    // RenderShadowMap();

    // Bind main back buffer
    auto* pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
    auto* pDSV = m_pSwapChain->GetDepthBufferDSV();
    m_pImmediateContext->SetRenderTargets(1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    // Clear the back buffer
    float4 ClearColor = {0.350f, 0.350f, 0.350f, 1.0f};

    m_pImmediateContext->ClearRenderTarget(pRTV, ClearColor.Data(), RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->ClearDepthStencil(pDSV, CLEAR_DEPTH_FLAG, 1.f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    std::for_each(m_queuedRenderObjects.begin(), m_queuedRenderObjects.end(), [this](const RenderData& renderData) { Render(renderData, false); } );
    // RenderShadowMapVis();

    // clear render queue
    m_queuedRenderObjects.clear();

    // Draw UI
    ServiceLocator::getUIManager().draw();

    // Present
    if (!m_pSwapChain) {
        return;
    }
    m_pSwapChain->Present(m_syncInterval);
}

size_t GameRendererDiligent::allocateModel(std::shared_ptr<ModelPrimitive> model)
{
    std::vector<StateTransitionDesc> barriers;
    std::vector<RefCntAutoPtr<IBuffer>> vertexBuffers, indexBuffers;
    std::vector<uint32_t> indexCounts;
    for(size_t i = 0; i < model->meshCount(); i++)
    {
        auto mesh = model->mesh(i);

        auto vertexBuffer = CreateVertexBuffer(m_pDevice, mesh, BUFFER_MODE_UNDEFINED);
        barriers.emplace_back(vertexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_VERTEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);
        vertexBuffers.emplace_back(vertexBuffer);

        auto indexBuffer  = CreateIndexBuffer(m_pDevice, mesh, BUFFER_MODE_UNDEFINED);
        barriers.emplace_back(indexBuffer, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_INDEX_BUFFER, STATE_TRANSITION_FLAG_UPDATE_STATE);        
        indexBuffers.emplace_back(indexBuffer);

        indexCounts.emplace_back(mesh->indexCount());
    }
    m_pImmediateContext->TransitionResourceStates(static_cast<Uint32>(barriers.size()), barriers.data());
    m_models.emplace_back(vertexBuffers, indexBuffers, indexCounts, model->meshCount());
    return m_models.size()-1;
}

RefCntAutoPtr<IBuffer> GameRendererDiligent::CreateVertexBuffer(Diligent::IRenderDevice* pDevice,
                                                                std::shared_ptr<MeshPrimitive> mesh,
                                                                Diligent::BUFFER_MODE Mode)
{
    const auto& pVertData = mesh->vertexData();

    BufferDesc VertBuffDesc;
    VertBuffDesc.Name      = "Vertex buffer";
    VertBuffDesc.Usage     = USAGE_IMMUTABLE;
    VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
    VertBuffDesc.Size      = pVertData.size();
    VertBuffDesc.Mode      = Mode;
    if (Mode != BUFFER_MODE_UNDEFINED)
    {
        VertBuffDesc.ElementByteStride = pVertData.size() / mesh->vertexCount();
    }

    BufferData VBData;
    VBData.pData    = pVertData.data();
    VBData.DataSize = VertBuffDesc.Size;
    RefCntAutoPtr<IBuffer> pCubeVertexBuffer;

    pDevice->CreateBuffer(VertBuffDesc, &VBData, &pCubeVertexBuffer);

    return pCubeVertexBuffer;
}

RefCntAutoPtr<IBuffer> GameRendererDiligent::CreateIndexBuffer(Diligent::IRenderDevice* pDevice,
                                                               std::shared_ptr<MeshPrimitive> mesh,
                                                               Diligent::BUFFER_MODE Mode)
{
    const auto& pIndexData = mesh->indexData();

    BufferDesc IndBuffDesc;
    IndBuffDesc.Name      = "Index buffer";
    IndBuffDesc.Usage     = USAGE_IMMUTABLE;
    IndBuffDesc.BindFlags = BIND_INDEX_BUFFER;
    IndBuffDesc.Size      = pIndexData.size();
    IndBuffDesc.Mode      = Mode;
    if (Mode != BUFFER_MODE_UNDEFINED)
        IndBuffDesc.ElementByteStride = pIndexData.size() / mesh->indexCount();
    BufferData IBData;
    IBData.pData    = pIndexData.data();
    IBData.DataSize = pIndexData.size();
    RefCntAutoPtr<IBuffer> pBuffer;
    pDevice->CreateBuffer(IndBuffDesc, &IBData, &pBuffer);
    return pBuffer;
}

size_t GameRendererDiligent::CreateColorMaterial(const glm::vec3& color, const std::string& name)
{
    // skip for now
    return (size_t)-1;
}

size_t GameRendererDiligent::CreateTextureMaterial(const std::string& texture, const std::string& name, bool isSharp)
{
    auto res = ServiceLocator::getResourceManager().get(texture);

    int width, height, numChannels;
    auto* pBuff = SOIL_load_image_from_memory((const unsigned char*)res->data.data(), res->data.size(), &width, &height, &numChannels, SOIL_LOAD_AUTO);
    if(pBuff == nullptr) 
    {
        spdlog::error("Failed to load image data from memory: {}", SOIL_last_result());
        return static_cast<size_t>(-1);
    }

    auto imgDataBlob = DataBlobImpl::Create(width * height * numChannels, pBuff);
    ImageDesc imgDesc;
    imgDesc.Width = width;
    imgDesc.Height = height;
    imgDesc.RowStride = width * numChannels;
    imgDesc.NumComponents = numChannels;
    imgDesc.ComponentType = VT_UINT8;
    RefCntAutoPtr<Image> pImg;
    Image::CreateFromMemory(imgDesc, imgDataBlob, &pImg);

    if(pImg == nullptr) {
        spdlog::error("Failed to create image from memory");
        return static_cast<size_t>(-1);
    }

    TextureLoadInfo texLoadInfo;
    texLoadInfo.IsSRGB = true;
    texLoadInfo.GenerateMips = true;
    texLoadInfo.MipLevels = 4;
    if (isSharp)
    {
        texLoadInfo.MipFilter = TEXTURE_LOAD_MIP_FILTER::TEXTURE_LOAD_MIP_FILTER_MOST_FREQUENT;
    }

    RefCntAutoPtr<ITextureLoader> pTexLoader;
    CreateTextureLoaderFromImage(pImg, texLoadInfo, &pTexLoader);

    RefCntAutoPtr<ITexture> pTex;
    pTexLoader->CreateTexture(m_pDevice, &pTex);

    if(pTex == nullptr) {
        spdlog::error("Failed to create texture from image");
        return (size_t)-1;
    }

    std::vector<StateTransitionDesc> Barriers;
    Barriers.emplace_back(pTex, RESOURCE_STATE_UNKNOWN, RESOURCE_STATE_SHADER_RESOURCE, STATE_TRANSITION_FLAG_UPDATE_STATE);
    m_pImmediateContext->TransitionResourceStates(static_cast<Uint32>(Barriers.size()), Barriers.data());

    RefCntAutoPtr<IShaderResourceBinding> pSRB;
    m_pCubePSO->CreateShaderResourceBinding(&pSRB, true);
    pSRB->GetVariableByName(SHADER_TYPE_PIXEL, "g_Texture")->Set(pTex->GetDefaultView(TEXTURE_VIEW_SHADER_RESOURCE));
    m_materials.emplace_back(pSRB);

    SOIL_free_image_data(pBuff);

    return m_materials.size()-1;
}

const glm::ivec2& GameRendererDiligent::getSize() const
{
    return m_size;
}

void GameRendererDiligent::resize(const glm::ivec2 &size)
{
    m_size = size;
    // resize graphics
    m_pSwapChain->Resize(size.x, size.y);
}

void GameRendererDiligent::updateCameraData(Camera3D &cam)
{
    const auto& viewmx = cam.getViewMatrix();
    memcpy(m_viewMatrix.Data(), &viewmx[0][0], sizeof(float) * 16);
    auto& projmx = cam.getProjectionMatrix();
    memcpy(m_projectionMatrix.Data(), &projmx[0][0], sizeof(float) * 16);
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

RefCntAutoPtr<ITexture> GameRendererDiligent::GenerateEmptyTexture()
{
    RefCntAutoPtr<ITexture> pTex;

    ImageDesc imgDesc;
    imgDesc.Width = 64;
    imgDesc.Height = 64;
    imgDesc.RowStride = imgDesc.Width * 1 * 3; // 64 in width, 1 byte per pixel, 3 components
    imgDesc.NumComponents = 3;
    imgDesc.ComponentType = VT_UINT8;

    RefCntAutoPtr<Image> pImg;
    std::vector<uint8_t> imgData(imgDesc.Width * imgDesc.Height * 3, 0);
    for(uint32_t i=0; i < imgDesc.Height; i++)
    {
        for(uint32_t j=0; j < imgDesc.Width; j++)
        {
            bool flip = (i + j) % 2 == 0;
            imgData[(i * imgDesc.Width + j) * 3 + 0] = flip ? 255 : 0;
            imgData[(i * imgDesc.Width + j) * 3 + 1] = 0;
            imgData[(i * imgDesc.Width + j) * 3 + 2] = flip ? 255 : 0;
        }
    }
    auto pImgDataBlob = DataBlobImpl::Create(imgData.size(), imgData.data());
    Image::CreateFromMemory(imgDesc, pImgDataBlob, &pImg);

    TextureLoadInfo texLoadInfo;
    texLoadInfo.IsSRGB = false;
    texLoadInfo.GenerateMips = false;

    RefCntAutoPtr<ITextureLoader> pTexLoader;
    CreateTextureLoaderFromImage(pImg, texLoadInfo, &pTexLoader);
    pTexLoader->CreateTexture(m_pDevice, &pTex);

    return pTex;
}

// TEMPORARY
namespace Diligent
{

namespace TexturedCube
{

RefCntAutoPtr<IPipelineState> CreatePipelineState(const CreatePSOInfo& CreateInfo, bool ConvertPSOutputToGamma)
{
    GraphicsPipelineStateCreateInfo PSOCreateInfo;
    PipelineStateDesc&              PSODesc          = PSOCreateInfo.PSODesc;
    PipelineResourceLayoutDesc&     ResourceLayout   = PSODesc.ResourceLayout;
    GraphicsPipelineDesc&           GraphicsPipeline = PSOCreateInfo.GraphicsPipeline;

    // This is a graphics pipeline
    PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    // Pipeline state name is used by the engine to report issues.
    // It is always a good idea to give objects descriptive names.
    PSODesc.Name = "Cube PSO";

    // clang-format off
    // This tutorial will render to a single render target
    GraphicsPipeline.NumRenderTargets             = 1;
    // Set render target format which is the format of the swap chain's color buffer
    GraphicsPipeline.RTVFormats[0]                = CreateInfo.RTVFormat;
    // Set depth buffer format which is the format of the swap chain's back buffer
    GraphicsPipeline.DSVFormat                    = CreateInfo.DSVFormat;
    // Set the desired number of samples
    GraphicsPipeline.SmplDesc.Count               = CreateInfo.SampleCount;
    // Primitive topology defines what kind of primitives will be rendered by this pipeline state
    GraphicsPipeline.PrimitiveTopology            = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    // Cull back faces
    GraphicsPipeline.RasterizerDesc.FrontCounterClockwise = True;
    GraphicsPipeline.RasterizerDesc.CullMode      = CULL_MODE_BACK;
    // Enable depth testing
    GraphicsPipeline.DepthStencilDesc.DepthEnable = True;
    // clang-format on
    ShaderCreateInfo ShaderCI;
    // Tell the system that the shader source code is in HLSL.
    // For OpenGL, the engine will convert this into GLSL under the hood.
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    // OpenGL backend requires emulated combined HLSL texture samplers (g_Texture + g_Texture_sampler combination)
    ShaderCI.Desc.UseCombinedTextureSamplers = true;

    // Presentation engine always expects input in gamma space. Normally, pixel shader output is
    // converted from linear to gamma space by the GPU. However, some platforms (e.g. Android in GLES mode,
    // or Emscripten in WebGL mode) do not support gamma-correction. In this case the application
    // has to do the conversion manually.
    ShaderMacro Macros[] = {{"CONVERT_PS_OUTPUT_TO_GAMMA", ConvertPSOutputToGamma ? "1" : "0"}};
    ShaderCI.Macros      = {Macros, _countof(Macros)};

    ShaderCI.pShaderSourceStreamFactory = CreateInfo.pShaderSourceFactory;
    // Create a vertex shader
    RefCntAutoPtr<IShader> pVS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "Cube VS";
        ShaderCI.FilePath        = CreateInfo.VSFilePath;
        CreateInfo.pDevice->CreateShader(ShaderCI, &pVS);
    }

    // Create a pixel shader
    RefCntAutoPtr<IShader> pPS;
    {
        ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "Cube PS";
        ShaderCI.FilePath        = CreateInfo.PSFilePath;
        CreateInfo.pDevice->CreateShader(ShaderCI, &pPS);
    }

    InputLayoutDescX InputLayout;

    Uint32 Attrib = 0;
    if (CreateInfo.Components & VERTEX_COMPONENT_FLAG_POSITION)
        InputLayout.Add(Attrib++, 0u, 3u, VT_FLOAT32, False);
    if (CreateInfo.Components & VERTEX_COMPONENT_FLAG_NORMAL)
        InputLayout.Add(Attrib++, 0u, 3u, VT_FLOAT32, False);
    if (CreateInfo.Components & VERTEX_COMPONENT_FLAG_TEXCOORD)
        InputLayout.Add(Attrib++, 0u, 2u, VT_FLOAT32, False);
    InputLayout.Add(Attrib++, 0u, 1u, VT_UINT32, False); // MeshId

    for (Uint32 i = 0; i < CreateInfo.NumExtraLayoutElements; ++i)
        InputLayout.Add(CreateInfo.ExtraLayoutElements[i]);

    GraphicsPipeline.InputLayout = InputLayout;

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    // Define variable type that will be used by default
    ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    // Shader variables should typically be mutable, which means they are expected
    // to change on a per-instance basis
    // clang-format off
    ShaderResourceVariableDesc Vars[] = 
    {
        {SHADER_TYPE_PIXEL, "g_Texture", SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
    };
    // clang-format on
    ResourceLayout.Variables    = Vars;
    ResourceLayout.NumVariables = _countof(Vars);

    // Define immutable sampler for g_Texture. Immutable samplers should be used whenever possible
    // clang-format off
    SamplerDesc SamLinearClampDesc
    {
        FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, FILTER_TYPE_LINEAR, 
        TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP, TEXTURE_ADDRESS_CLAMP
    };
    ImmutableSamplerDesc ImtblSamplers[] = 
    {
        {SHADER_TYPE_PIXEL, "g_Texture", SamLinearClampDesc}
    };
    // clang-format on
    ResourceLayout.ImmutableSamplers    = ImtblSamplers;
    ResourceLayout.NumImmutableSamplers = _countof(ImtblSamplers);

    RefCntAutoPtr<IPipelineState> pPSO;
    CreateInfo.pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &pPSO);
    return pPSO;
}

} // namespace TexturedCube

} // namespace Diligent