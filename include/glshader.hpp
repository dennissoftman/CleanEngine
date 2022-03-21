#ifndef GLSHADER_HPP
#define GLSHADER_HPP

#include "shader.hpp"
#include <string>
#include <GL/glew.h>

class GLShader : public Shader
{
public:
    GLShader();

    void load(const std::string &vs, const std::string &fs) override;

    void use() override;
private:
    GLuint m_pid;
};

#endif // GLSHADER_HPP
