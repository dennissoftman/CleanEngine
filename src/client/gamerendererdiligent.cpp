#include "client/gamerendererdiligent.hpp"

GameRendererDiligent::GameRendererDiligent()
{

}

void GameRendererDiligent::init(const VideoMode &mode)
{
    m_size = glm::ivec2(mode.width, mode.height);
}

void GameRendererDiligent::queueRenderObject(const Model3D *obj, const glm::mat4 &modelMatrix)
{

}

void GameRendererDiligent::draw()
{

}

const glm::ivec2& GameRendererDiligent::getSize() const
{
    return m_size;
}

void GameRendererDiligent::resize(const glm::ivec2 &size)
{
    m_size = size;
    // resize graphics
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
