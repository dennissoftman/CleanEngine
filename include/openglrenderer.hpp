#ifndef OPENGLRENDERER_HPP
#define OPENGLRENDERER_HPP

#include "renderer.hpp"
#include <GL/glew.h>

struct GLRenderObject
{
    RenderObject *parent;

    GLuint vao, vbo;
};

class OpenGLRenderer : public Renderer
{
public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    void init(const VideoMode &mode) override;

    void queueRenderObject(RenderObject *obj) override;
    void queueRenderObject(GLRenderObject obj);
    void draw() override;

    void setProjectionMatrix(const glm::mat4 &projmx) override;
    void setViewMatrix(const glm::mat4 &viewmx) override;
private:
    GLRenderObject createRenderObject(RenderObject *obj);

    GLuint m_VAO, m_VBO;
    std::vector<GLRenderObject> m_createdObjects;
    std::queue<GLRenderObject> m_renderQueue;
    glm::mat4 m_projMatrix, m_viewMatrix;

    bool m_was_init;
};

#endif // OPENGLRENDERER_HPP
