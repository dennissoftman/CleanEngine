#include "openglrenderer.hpp"
#include "shader.hpp"
#include <cstdio>
#include <cassert>
#include "servicelocator.hpp"

const std::string MODULE_NAME = "OpenGLRenderer";

OpenGLRenderer::OpenGLRenderer()
    : m_projMatrix(glm::mat4(1)), m_was_init(false)
{

}

OpenGLRenderer::~OpenGLRenderer()
{
    for(const GLRenderObject &obj : m_createdObjects)
    {
        glDeleteVertexArrays(1, &obj.vao);
        glDeleteBuffers(1, &obj.vbo);
    }
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

    m_was_init = true;
}

void OpenGLRenderer::queueRenderObject(RenderObject *obj)
{
    for(const GLRenderObject &gObj : m_createdObjects)
    {
        if(gObj.parent == obj)
        {
            queueRenderObject(gObj);
            return;
        }
    }
    GLRenderObject gObj = createRenderObject(obj);
    m_createdObjects.push_back(gObj);
    queueRenderObject(gObj);
}

void OpenGLRenderer::queueRenderObject(GLRenderObject obj)
{
    m_renderQueue.push(obj);
}

void OpenGLRenderer::draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    while(m_renderQueue.size() > 0)
    {
        GLRenderObject obj = m_renderQueue.front();
        if(obj.parent == NULL)
        {
            m_renderQueue.pop();
            continue;
        }

        obj.parent->mat->use(TransformData{
                                 .Projection = m_projMatrix,
                                 .View = m_viewMatrix,
                                 .Model = obj.parent->modelMatrix
                             });

        glBindVertexArray(obj.vao);
            glDrawArrays(GL_TRIANGLES, 0, obj.parent->model->tris.size()*3);
        glBindVertexArray(0);
        m_renderQueue.pop();
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLRenderer::setProjectionMatrix(const glm::mat4 &projmx)
{
    m_projMatrix = projmx;
}

void OpenGLRenderer::setViewMatrix(const glm::mat4 &viewmx)
{
    m_viewMatrix = viewmx;
}

GLRenderObject OpenGLRenderer::createRenderObject(RenderObject *obj)
{
    GLRenderObject gObj;
    gObj.parent = obj;

    glGenVertexArrays(1, &gObj.vao);
    glGenBuffers(1, &gObj.vbo);

    glBindVertexArray(gObj.vao);
    glBindBuffer(GL_ARRAY_BUFFER, gObj.vbo);

    glBufferData(GL_ARRAY_BUFFER,
                 obj->model->tris.size() * sizeof(Triangle3D),
                 obj->model->tris.data(),
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          5*sizeof(GLfloat),
                          (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          5*sizeof(GLfloat),
                          (GLvoid*)(3*sizeof(GLfloat)));

    glBindVertexArray(0);

    return gObj;
}
