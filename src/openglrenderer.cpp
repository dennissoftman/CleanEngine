#include "openglrenderer.hpp"
#include "shader.hpp"
#include <cstdio>
#include <cassert>
#include "servicelocator.hpp"

const std::string MODULE_NAME = "OpenGLRenderer";

OpenGLRenderer::OpenGLRenderer()
    : m_currentShader(NULL), m_was_init(false)
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

    glViewport(0, 0, mode.width, mode.height);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

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
    glClear(GL_COLOR_BUFFER_BIT);

    while(m_renderQueue.size() > 0)
    {
        GLRenderObject obj = m_renderQueue.front();
        if(obj.parent == NULL)
        {
            m_renderQueue.pop();
            continue;
        }
        obj.parent->shader->use();
        obj.parent->shader->setMat4("modelMatrix", obj.parent->modelMatrix);
        glBindVertexArray(obj.vao);
            glDrawArrays(GL_TRIANGLES, 0, obj.parent->model->tris.size()*3);
        glBindVertexArray(0);
        m_renderQueue.pop();
    }
}

void OpenGLRenderer::setShader(Shader &shader)
{
    m_currentShader = &shader;
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
                 obj->model->tris.size() * sizeof(Triangle2D),
                 obj->model->tris.data(),
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          5*sizeof(GLfloat),
                          (GLvoid*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          5*sizeof(GLfloat),
                          (GLvoid*)(2*sizeof(GLfloat)));

    glBindVertexArray(0);

    return gObj;
}
