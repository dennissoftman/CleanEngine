#include "openglrenderer.hpp"
#include "glmaterial.hpp"
#include "servicelocator.hpp"

#include <cstdio>
#include <cassert>

static const std::string MODULE_NAME = "OpenGLRenderer";

OpenGLRenderer::OpenGLRenderer()
    : m_projMatrix(glm::mat4(1)),
      m_was_init(false),
      m_defaultMaterial(nullptr)
{

}

OpenGLRenderer::~OpenGLRenderer()
{
    while(!m_renderQueue.empty())
        m_renderQueue.pop();

    for(GLRenderObject *obj : m_createdObjects)
        delete obj;

    delete m_defaultMaterial;
    ServiceLocator::getLogger().info(MODULE_NAME, "Successful cleanup");
}

void OpenGLRenderer::init(const VideoMode &mode)
{
    assert (!m_was_init && "Can't init renderer twice");

    glewExperimental = GL_TRUE;

    GLenum err = glewInit();
    if(err != GLEW_OK)
    {
        ServiceLocator::getLogger().error(MODULE_NAME, "Failed to init GLEW");
        ServiceLocator::getLogger().error(MODULE_NAME, (const char*)glewGetErrorString(err));
        throw std::runtime_error("Failed to init GLEW");
    }

    //
    const char *glVerStr = (const char*)glGetString(GL_VERSION);
    if(glVerStr)
        ServiceLocator::getLogger().info(MODULE_NAME, "Using OpenGL "+std::string(glVerStr));
    else
        ServiceLocator::getLogger().info(MODULE_NAME, "Using unknown OpenGL version");
    //

    glViewport(0, 0, mode.width, mode.height);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.f, 1.f);

    glEnable(GL_MULTISAMPLE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // fallback material
    m_defaultMaterial = new GLMaterial();
    m_defaultMaterial->init();
    //

    m_was_init = true;
    ServiceLocator::getLogger().info(MODULE_NAME, "Init complete");
}

void OpenGLRenderer::queueRenderObject(const Model3D *obj, const glm::mat4 &modelMatrix)
{
    for(GLRenderObject *gObj : m_createdObjects)
    {
        if(gObj->parent == obj)
        {
            queueRenderObject(gObj, modelMatrix);
            return;
        }
    }
    GLRenderObject *gObj = createRenderObject(obj);
    m_createdObjects.push_back(gObj);
    queueRenderObject(gObj, modelMatrix);
}

void OpenGLRenderer::queueRenderObject(GLRenderObject *obj, const glm::mat4 &modelMatrix)
{
    m_renderQueue.push(GLRenderRequest{.renderObject = obj, .modelMatrix = modelMatrix});
}

void OpenGLRenderer::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    while(!m_renderQueue.empty())
    {
        GLRenderRequest req = m_renderQueue.front();
        if(req.renderObject == nullptr)
        {
            m_renderQueue.pop();
            continue;
        }

        GLRenderObject *obj = req.renderObject;
        if(obj->parent == nullptr)
        {
            m_renderQueue.pop();
            continue;
        }

        for(size_t i=0; i < obj->meshCount; i++)
        {
            TransformData td =
            {
                .Projection = m_projMatrix,
                .View = m_viewMatrix,
                .Model = req.modelMatrix
            };

            if (obj->parent->pMat != nullptr)
                obj->parent->pMat->use(td);
            else
                m_defaultMaterial->use(td);

            glBindVertexArray(obj->VAOs[i]);
            glDrawArrays(GL_TRIANGLES, 0, obj->parent->pMeshes[i].tris.size()*3);
            glBindVertexArray(0);
        }
        //

        m_renderQueue.pop();
    }
}

glm::ivec2 OpenGLRenderer::getSize() const
{
    return m_currentSize;
}

void OpenGLRenderer::resize(const glm::ivec2 &size)
{
    m_currentSize = size;
}

void OpenGLRenderer::setProjectionMatrix(const glm::mat4 &projmx)
{
    m_projMatrix = projmx;
}

void OpenGLRenderer::setViewMatrix(const glm::mat4 &viewmx)
{
    m_viewMatrix = viewmx;
}

std::string OpenGLRenderer::getType() const
{
    return OPENGL_RENDERER_TYPE;
}

GLRenderObject *OpenGLRenderer::createRenderObject(const Model3D *obj)
{
    return new GLRenderObject(obj);
}
