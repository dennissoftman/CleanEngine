#include "dummyrenderer.hpp"
#include "servicelocator.hpp"

static const char *MODULE_NAME = "DummyRenderer";

DummyRenderer::DummyRenderer()
= default;

DummyRenderer::~DummyRenderer()
= default;

void DummyRenderer::init(const VideoMode &mode)
{
    Logger &logger = ServiceLocator::getLogger();

    std::stringstream infostr;
    infostr << "VideoMode: (" << mode.width << "x" << mode.height << ")";
    logger.info(MODULE_NAME, infostr.str());
}

void DummyRenderer::queueRenderObject(const Model3D *obj, const glm::mat4 &modelMatrix)
{
    (void)obj;
    (void)modelMatrix;
}

void DummyRenderer::draw()
{

}

void DummyRenderer::resize(const glm::ivec2 &size)
{
    (void)size;
}

void DummyRenderer::setViewMatrix(const glm::mat4 &viewmx)
{
    (void)viewmx;
}

void DummyRenderer::setProjectionMatrix(const glm::mat4 &projmx)
{
    (void)projmx;
}

std::string DummyRenderer::getType() const
{
    return "dummy";
}

glm::ivec2 DummyRenderer::getSize() const
{
    return {};
}

