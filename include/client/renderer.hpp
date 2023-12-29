#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "common/modelmanager.hpp"

#include <queue>
#include <glm/glm.hpp>
#include <stdexcept>

class VideoMode
{
public:
    VideoMode() 
    { }

    VideoMode& setWidth(int w)
    {
        m_width = w;
        return *this;
    }

    int width() const
    {
        return m_width;
    }

    VideoMode& setHeight(int h)
    {
        m_height = h;
        return *this;
    }

    int height() const
    {
        return m_height;
    }

    VideoMode& setFullscreen(bool f)
    {
        m_fullscreen = f;
        return *this;
    }

    bool fullscreen() const
    {
        return m_fullscreen;
    }

    VideoMode& setVsync(bool v)
    {
        m_vsync = v;
        return *this;
    }

    bool vsync() const
    {
        return m_vsync;
    }

    VideoMode& setSamples(int s)
    {
        m_samples = s;
        return *this;
    }

    int samples() const
    {
        return m_samples;
    }

    VideoMode& setRenderingBackend(const std::string &backend)
    {
        if(backend != "vk" && backend != "dx12")
            throw std::invalid_argument("Invalid rendering backend: " + backend + " (only vk and dx12 are supported)");
        m_renderingBackend = backend;
        return *this;
    }

    const std::string& renderingBackend() const
    {
        return m_renderingBackend;
    }

    VideoMode& setShadowMapResolution(int res)
    {
        m_shadowMapResolution = res;
        return *this;
    }

    int shadowMapResolution() const
    {
        return m_shadowMapResolution;
    }

    VideoMode& setFSRScaling(float scale)
    {
        m_fsrScaling = scale;
        return *this;
    }

    float fsrScaling() const
    {
        return m_fsrScaling;
    }

    VideoMode& setOSData(void *data)
    {
        m_osdata = data;
        return *this;
    }

    void* osdata() const
    {
        return m_osdata;
    }
private:
    int m_width, m_height;
    bool m_fullscreen=false, m_vsync=false;
    int m_samples=1;
    std::string m_renderingBackend; // vk, dx12
    int m_shadowMapResolution=512;
    float m_fsrScaling=1.0f;

    void* m_osdata; // HWND or xcb_display_t
};

class Camera3D;
class MaterialManager;

class Renderer
{
    friend class MaterialManager;
public:
    static Renderer *create();
    virtual ~Renderer() {}

    virtual void init(const VideoMode &mode) = 0;

    virtual void update(double dt) = 0;
    virtual void draw() = 0;

    virtual const glm::ivec2& getSize() const = 0;
    virtual void resize(const glm::ivec2 &size) = 0;

    virtual void updateCameraData(Camera3D &cam) = 0;
    virtual void updateLightCount(uint32_t count) = 0;
    virtual void updateLightPosition(const glm::vec4 &pos, uint32_t id=0) = 0;
    virtual void updateLightColor(const glm::vec4 &color, uint32_t id=0) = 0;

    virtual void queueRender(size_t modelId, const std::vector<size_t>& materialIds, const glm::mat4& modelMatrix) = 0;
    virtual void queueRender(size_t modelId, const std::vector<size_t>& materialIds, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& meshMatrices) = 0;
    virtual size_t allocateModel(std::shared_ptr<ModelPrimitive> mesh) = 0;

    // lowercase renderer identifier (gl, vk, dx, etc.)
    virtual std::string getType() const = 0;
protected:
    virtual size_t CreateColorMaterial(const glm::vec3& color, const std::string& name) = 0;
    virtual size_t CreateTextureMaterial(const std::string& path, const std::string& name, bool isSharp=false) = 0;
};

#endif // RENDERER_HPP
