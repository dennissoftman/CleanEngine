#ifndef OPENGLRENDERER_HPP
#define OPENGLRENDERER_HPP

#include "renderer.hpp"
#include <GL/glew.h>

class OpenGLRenderer : public Renderer
{
public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    void init(const VideoMode &mode) override;

    void draw() override;

    void setShader(Shader &shader);
private:
    Shader *m_currentShader;
    bool m_was_init;
};

#endif // OPENGLRENDERER_HPP
