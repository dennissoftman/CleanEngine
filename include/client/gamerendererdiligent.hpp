#ifndef GAMERENDERERDILIGENT_HPP
#define GAMERENDERERDILIGENT_HPP

#include <variant>

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
#include "common/entities/meshcomponent.hpp"

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

struct ModelData
{
    std::vector<Diligent::RefCntAutoPtr<Diligent::IBuffer>> vertexBuffers;
    std::vector<Diligent::RefCntAutoPtr<Diligent::IBuffer>> indexBuffers;
    std::vector<uint32_t> indexCounts;
    size_t meshCount;
};

struct RenderData 
{
    size_t modelId;
    std::vector<size_t> materialIds;
    Diligent::float4x4 modelMatrix;
    std::vector<Diligent::float4x4> meshMatrices;
};

enum VERTEX_COMPONENT_FLAGS : Diligent::Uint32
{
    VERTEX_COMPONENT_FLAG_NONE     = 0x00,
    VERTEX_COMPONENT_FLAG_POSITION = 0x01,
    VERTEX_COMPONENT_FLAG_NORMAL   = 0x02,
    VERTEX_COMPONENT_FLAG_TEXCOORD = 0x04,

    VERTEX_COMPONENT_FLAG_POS_UV =
        VERTEX_COMPONENT_FLAG_POSITION |
        VERTEX_COMPONENT_FLAG_TEXCOORD,

    VERTEX_COMPONENT_FLAG_POS_NORM_UV =
        VERTEX_COMPONENT_FLAG_POSITION |
        VERTEX_COMPONENT_FLAG_NORMAL |
        VERTEX_COMPONENT_FLAG_TEXCOORD
};
DEFINE_FLAG_ENUM_OPERATORS(VERTEX_COMPONENT_FLAGS);

class GameRendererDiligent : public Renderer
{
    friend class UIManager;
public:
    GameRendererDiligent();

    void init(const VideoMode &mode) override;
    void update(double dt) override;
    void draw() override;
    const glm::ivec2& getSize() const override;
    void resize(const glm::ivec2 &size) override;
    void updateCameraData(Camera3D &cam) override;
    void updateLightCount(uint32_t count) override;
    void updateLightPosition(const glm::vec4 &pos, uint32_t id) override;
    void updateLightColor(const glm::vec4 &color, uint32_t id) override;
    std::string getType() const override;

    void queueRender(size_t modelId, const std::vector<size_t>& materialIds, const glm::mat4& modelMatrix) override;
    void queueRender(size_t modelId, const std::vector<size_t>& materialIds, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& meshMatrices) override;

    size_t allocateModel(std::shared_ptr<ModelPrimitive> mesh) override;
protected:
    size_t CreateTextureMaterial(const std::string& path, const std::string& name, bool isSharp=false) override;
    size_t CreateColorMaterial(const glm::vec3& color, const std::string& name) override;
private:
    Diligent::RefCntAutoPtr<Diligent::IBuffer> CreateVertexBuffer(Diligent::IRenderDevice* pDevice,
                                                                  std::shared_ptr<MeshPrimitive> mesh,
                                                                  Diligent::BUFFER_MODE Mode);
    Diligent::RefCntAutoPtr<Diligent::IBuffer> CreateIndexBuffer(Diligent::IRenderDevice* pDevice,
                                                                 std::shared_ptr<MeshPrimitive> mesh,
                                                                 Diligent::BUFFER_MODE Mode);

    DiligentEngineRendererData InitializeDiligentEngine(Diligent::SwapChainDesc SCDesc, Diligent::RENDER_DEVICE_TYPE deviceType);
    void CreateCubeShadowPSO();
    void CreateTexturedPSO();
    void CreateShadowMapVisPSO();
    void CreateShadowMap();

    void RenderShadowMap();
    void Render(const RenderData& renderData, bool IsShadowPass);
    void RenderShadowMapVis();

    Diligent::RefCntAutoPtr<Diligent::ITexture> GenerateEmptyTexture();

    glm::ivec2 m_size;
    VideoMode m_videoMode;
    Diligent::Uint32 m_syncInterval = 1;

    std::vector<ModelData> m_models;
    std::vector<Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding>> m_materials;
    std::vector<Diligent::RefCntAutoPtr<Diligent::ITexture>> m_textures;

    std::vector<RenderData> m_queuedRenderObjects;

    Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_pDevice;
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_pImmediateContext;
    Diligent::RefCntAutoPtr<Diligent::ISwapChain> m_pSwapChain;
    Diligent::RefCntAutoPtr<Diligent::IBuffer> m_VSConstants;
    Diligent::RefCntAutoPtr<Diligent::IBuffer> m_CubeVertexBuffer;
    Diligent::RefCntAutoPtr<Diligent::IBuffer> m_CubeIndexBuffer;
    Diligent::RefCntAutoPtr<Diligent::IEngineFactory> m_pEngineFactory;
    Diligent::float4x4 m_projectionMatrix;
    Diligent::float4x4 m_viewMatrix;
    double m_elapsedTime;
    Diligent::float4x4 GetSurfacePretransformMatrix(const Diligent::float3& f3CameraViewAxis) const;
    Diligent::float4x4 GetAdjustedProjectionMatrix(float FOV, float NearPlane, float FarPlane) const;
    // TEMPORARY
    Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pCubePSO;
    Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_defaultSRB;
    Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pCubeShadowPSO;
    Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_CubeShadowSRB;

    Diligent::RefCntAutoPtr<Diligent::IPipelineState> m_pShadowMapVisPSO;
    Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> m_ShadowMapVisSRB;
    Diligent::Uint32 m_ShadowMapSize = 512;
    Diligent::TEXTURE_FORMAT m_ShadowMapFormat = Diligent::TEX_FORMAT_D16_UNORM;
    Diligent::RefCntAutoPtr<Diligent::ITextureView> m_ShadowMapDSV;
    Diligent::RefCntAutoPtr<Diligent::ITextureView> m_ShadowMapSRV;

    Diligent::float4x4 m_CubeWorldMatrix;
    Diligent::float4x4 m_CameraViewProjMatrix;
    Diligent::float4x4 m_WorldToShadowMapUVDepthMatr;
    Diligent::float3 m_LightDirection = Diligent::normalize(Diligent::float3(-0.49f, -0.60f, 0.64f));
};

// TEMPORARY
namespace Diligent {
namespace TexturedCube
{

enum VERTEX_COMPONENT_FLAGS : Uint32
{
    VERTEX_COMPONENT_FLAG_NONE     = 0x00,
    VERTEX_COMPONENT_FLAG_POSITION = 0x01,
    VERTEX_COMPONENT_FLAG_NORMAL   = 0x02,
    VERTEX_COMPONENT_FLAG_TEXCOORD = 0x04,

    VERTEX_COMPONENT_FLAG_POS_UV =
        VERTEX_COMPONENT_FLAG_POSITION |
        VERTEX_COMPONENT_FLAG_TEXCOORD,

    VERTEX_COMPONENT_FLAG_POS_NORM_UV =
        VERTEX_COMPONENT_FLAG_POSITION |
        VERTEX_COMPONENT_FLAG_NORMAL |
        VERTEX_COMPONENT_FLAG_TEXCOORD
};
DEFINE_FLAG_ENUM_OPERATORS(VERTEX_COMPONENT_FLAGS);

RefCntAutoPtr<IBuffer>  CreateVertexBuffer(IRenderDevice*         pDevice,
                                           VERTEX_COMPONENT_FLAGS Components,
                                           BIND_FLAGS             BindFlags = BIND_VERTEX_BUFFER,
                                           BUFFER_MODE            Mode      = BUFFER_MODE_UNDEFINED);
RefCntAutoPtr<IBuffer>  CreateIndexBuffer(IRenderDevice* pDevice,
                                          BIND_FLAGS     BindFlags = BIND_INDEX_BUFFER,
                                          BUFFER_MODE    Mode      = BUFFER_MODE_UNDEFINED);
RefCntAutoPtr<ITexture> LoadTexture(IRenderDevice* pDevice, const char* Path);

struct CreatePSOInfo
{
    IRenderDevice*                   pDevice                = nullptr;
    TEXTURE_FORMAT                   RTVFormat              = TEX_FORMAT_UNKNOWN;
    TEXTURE_FORMAT                   DSVFormat              = TEX_FORMAT_UNKNOWN;
    IShaderSourceInputStreamFactory* pShaderSourceFactory   = nullptr;
    const char*                      VSFilePath             = nullptr;
    const char*                      PSFilePath             = nullptr;
    VERTEX_COMPONENT_FLAGS           Components             = VERTEX_COMPONENT_FLAG_NONE;
    LayoutElement*                   ExtraLayoutElements    = nullptr;
    Uint32                           NumExtraLayoutElements = 0;
    Uint8                            SampleCount            = 1;
};
RefCntAutoPtr<IPipelineState> CreatePipelineState(const CreatePSOInfo& CreateInfo, bool ConvertPSOutputToGamma = false);

static constexpr Uint32 NumVertices = 4 * 6;
static constexpr Uint32 NumIndices  = 3 * 2 * 6;

extern const std::array<float3, NumVertices> Positions;
extern const std::array<float2, NumVertices> Texcoords;
extern const std::array<float3, NumVertices> Normals;

extern const std::array<Uint32, NumIndices> Indices;

} // namespace TexturedCube
} // namespace Diligent
// =========

#endif // GAMERENDERERDILIGENT_HPP
