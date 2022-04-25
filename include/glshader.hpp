#ifndef GLSHADER_HPP
#define GLSHADER_HPP

#include "shader.hpp"
#include <string>
#include <GL/glew.h>

class GLShader : public Shader
{
public:
    GLShader();

    void load(const char *vdata, int vsize, const char *fdata, int fsize) override;
    void load(const std::string &vs, const std::string &fs) override;

    void use() override;

    void setMat4(const std::string_view &id, const glm::mat4 &mat) override;
    void setInt(const std::string_view &id, int a) override;
private:
    GLuint m_pid;
};

#endif // GLSHADER_HPP
