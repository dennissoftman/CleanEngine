#include "openglrenderer.hpp"
#include "shader.hpp"
#include <cstdio>
#include <cassert>

static const GLfloat modelData[] = {
    -0.5f, -0.5f,   1.f, 0.f, 0.f,
    0.0f, 0.5f,     0.f, 1.f, 0.f,
    0.5f, -0.5f,    0.f, 0.f, 1.f
};
GLuint VAO, VBO;

OpenGLRenderer::OpenGLRenderer()
    : m_currentShader(NULL), m_was_init(false)
{

}

OpenGLRenderer::~OpenGLRenderer()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void OpenGLRenderer::init(const VideoMode &mode)
{
    assert (!m_was_init && "Can't init renderer twice");
    glewExperimental = GL_TRUE;

    GLenum err = glewInit();
    if(err != GLEW_OK)
    {
        fprintf(stderr, "Failed to init glew:\n%s\n", glewGetErrorString(err));
        exit(1);
    }

    glViewport(0, 0, mode.width, mode.height);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);


    // TEMP

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(modelData), modelData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          5*sizeof(GLfloat),
                          (GLvoid*)0);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          5*sizeof(GLfloat),
                          (GLvoid*)(2*sizeof(GLfloat)));

    glBindVertexArray(0);

    m_was_init = true;
}

void OpenGLRenderer::draw()
{
    if(m_currentShader == NULL)
        return;

    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);
    m_currentShader->use();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void OpenGLRenderer::setShader(Shader &shader)
{
    m_currentShader = &shader;
}
