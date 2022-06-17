#ifndef DUMMYRENDERER_HPP
#define DUMMYRENDERER_HPP

#include "client/renderer.hpp"
#include "common/logger.hpp"

class DummyRenderer : public Renderer
{
public:
    DummyRenderer();
    ~DummyRenderer() override;

    void init(const VideoMode &mode) override;

    void queueRenderObject(const Model3D *obj, const glm::mat4 &modelMatrix) override;
    void draw() override;

    [[nodiscard]] glm::ivec2 getSize() const override;
    void resize(const glm::ivec2 &size) override;

    void updateCameraData(Camera3D &cam) override;

    [[nodiscard]] std::string getType() const override;
};

#endif // DUMMYRENDERER_HPP
