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

    void setShader(Shader &shader);
private:
    GLRenderObject createRenderObject(RenderObject *obj);

    GLuint m_VAO, m_VBO;
    std::vector<GLRenderObject> m_createdObjects;
    std::queue<GLRenderObject> m_renderQueue;

    Shader *m_currentShader;
    bool m_was_init;
};

#endif // OPENGLRENDERER_HPP
