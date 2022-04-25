#include "dummyrenderer.hpp"
#include "servicelocator.hpp"

const std::string MODULE_NAME = "DummyRenderer";

DummyRenderer::DummyRenderer()
{

}

void DummyRenderer::init(const VideoMode &mode)
{
    m_gameLogger = &ServiceLocator::getLogger();

    m_gameLogger->info(MODULE_NAME,
                       "VideoMode: (" +
                       std::to_string(mode.width) + "x" +
                       std::to_string(mode.height) +")");
}

void DummyRenderer::queueRenderObject(RenderObject *obj)
{
    (void)obj;
}

void DummyRenderer::draw()
{

}

void DummyRenderer::setViewMatrix(const glm::mat4 &viewmx)
{
    (void)viewmx;
}

void DummyRenderer::setProjectionMatrix(const glm::mat4 &projmx)
{
    (void)projmx;
}
