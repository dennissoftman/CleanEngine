#ifndef DUMMYRENDERER_HPP
#define DUMMYRENDERER_HPP

#include "renderer.hpp"
#include "logger.hpp"

class DummyRenderer : public Renderer
{
public:
    DummyRenderer();

    void init(const VideoMode &mode) override;

    void queueRenderObject(const Model3D *obj, const glm::mat4 &modelMatrix) override;
    void draw() override;

    void setViewMatrix(const glm::mat4 &viewmx) override;
    void setProjectionMatrix(const glm::mat4 &projmx) override;
private:
    Logger *m_gameLogger;
};

#endif // DUMMYRENDERER_HPP
