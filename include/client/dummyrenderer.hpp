#ifndef DUMMYRENDERER_HPP
#define DUMMYRENDERER_HPP

#include "client/renderer.hpp"

class DummyRenderer : public Renderer
{
public:
    DummyRenderer();
    ~DummyRenderer() override;

    void init(const VideoMode &mode) override;

    void draw() override;

    const glm::ivec2& getSize() const override;
    void resize(const glm::ivec2 &size) override;

    void updateCameraData(Camera3D &cam) override;

    std::string getType() const override;
};

#endif // DUMMYRENDERER_HPP
