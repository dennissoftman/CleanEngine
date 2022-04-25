#include "glshader.hpp"
#include <vector>
#include <sstream>
#include <fstream>

#include "servicelocator.hpp"

static const char *MODULE_NAME = "GLShader";

GLShader::GLShader()
    : m_pid(0)
{

}

void GLShader::load(const char *vdata, int vsize, const char *fdata, int fsize)
{
    GLuint vs_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs_id = glCreateShader(GL_FRAGMENT_SHADER);
    GLint res, ilen;

    glShaderSource(vs_id, 1, &vdata, &vsize);
    glCompileShader(vs_id);

    glGetShaderiv(vs_id, GL_COMPILE_STATUS, &res);
    glGetShaderiv(vs_id, GL_INFO_LOG_LENGTH, &ilen);
    if(ilen > 0)
    {
        std::vector<char> pErrMsg(ilen+1);
        glGetShaderInfoLog(vs_id, ilen, NULL, pErrMsg.data());
        ServiceLocator::getLogger().error(MODULE_NAME, "Failed to load vertex shader: "+std::string(pErrMsg.data()));
    }

    glShaderSource(fs_id, 1, &fdata, &fsize);
    glCompileShader(fs_id);

    glGetShaderiv(fs_id, GL_COMPILE_STATUS, &res);
    glGetShaderiv(fs_id, GL_INFO_LOG_LENGTH, &ilen);
    if(ilen > 0)
    {
        std::vector<char> pErrMsg(ilen+1);
        glGetShaderInfoLog(fs_id, ilen, NULL, pErrMsg.data());
        ServiceLocator::getLogger().error(MODULE_NAME, "Failed to load fragment shader: "+std::string(pErrMsg.data()));
    }

    m_pid = glCreateProgram();
    glAttachShader(m_pid, vs_id);
    glAttachShader(m_pid, fs_id);
    glLinkProgram(m_pid);

    glGetProgramiv(m_pid, GL_LINK_STATUS, &res);
    glGetProgramiv(m_pid, GL_INFO_LOG_LENGTH, &ilen);
    if(ilen > 0)
    {
        std::vector<char> pErrMsg(ilen+1);
        glGetProgramInfoLog(m_pid, ilen, NULL, pErrMsg.data());
        ServiceLocator::getLogger().error(MODULE_NAME, "Failed to link shader program: "+std::string(pErrMsg.data()));
    }

    glDeleteShader(vs_id);
    glDeleteShader(fs_id);
}

void GLShader::load(const std::string &vs, const std::string &fs)
{
    std::string vshCode;
    std::ifstream vshFile(vs, std::ios::in);
    if(vshFile.is_open())
    {
        std::stringstream tmp;
        tmp << vshFile.rdbuf();
        vshFile.close();
        vshCode = tmp.str();
    }

    std::string fshCode;
    std::ifstream fshFile(fs, std::ios::in);
    if(fshFile.is_open())
    {
        std::stringstream tmp;
        tmp << fshFile.rdbuf();
        fshFile.close();
        fshCode = tmp.str();
    }

    load(vshCode.data(), vshCode.size(), fshCode.data(), fshCode.size());
}

void GLShader::use()
{
    glUseProgram(m_pid);
}

void GLShader::setMat4(const std::string_view &id, const glm::mat4 &mat)
{
    glUniformMatrix4fv(glGetUniformLocation(m_pid, id.data()), 1, GL_FALSE, &mat[0][0]);
}

void GLShader::setInt(const std::string_view &id, int a)
{
    glUniform1i(glGetUniformLocation(m_pid, id.data()), a);
}
