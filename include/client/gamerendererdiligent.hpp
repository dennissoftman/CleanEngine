#ifndef GAMERENDERERDILIGENT_HPP
#define GAMERENDERERDILIGENT_HPP

#include "client/renderer.hpp"

class GameRendererDiligent : public Renderer
{
public:
    GameRendererDiligent();

    void init(const VideoMode &mode);
    void queueRenderObject(const Model3D *obj, const glm::mat4 &modelMatrix);
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
};

#endif // GAMERENDERERDILIGENT_HPP
