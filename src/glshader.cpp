#include "glshader.hpp"
#include <vector>
#include <sstream>
#include <fstream>

GLShader::GLShader()
    : m_pid(0)
{

}

void GLShader::load(const std::string &vs, const std::string &fs)
{
    GLuint vs_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs_id = glCreateShader(GL_FRAGMENT_SHADER);

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

    GLint res;
    int ilen;

    char const *vshData = vshCode.c_str();
    glShaderSource(vs_id, 1, &vshData, NULL);
    glCompileShader(vs_id);

    glGetShaderiv(vs_id, GL_COMPILE_STATUS, &res);
    glGetShaderiv(vs_id, GL_INFO_LOG_LENGTH, &ilen);
    if(ilen > 0)
    {
        std::vector<char> pErrMsg(ilen+1);
        glGetProgramInfoLog(m_pid, ilen, NULL, pErrMsg.data());
        fprintf(stderr, "Failed to compile vertex shader:\n%s\n", pErrMsg.data());
    }

    char const *fshData = fshCode.c_str();
    glShaderSource(fs_id, 1, &fshData, NULL);
    glCompileShader(fs_id);

    glGetShaderiv(fs_id, GL_COMPILE_STATUS, &res);
    glGetShaderiv(fs_id, GL_INFO_LOG_LENGTH, &ilen);
    if(ilen > 0)
    {
        std::vector<char> pErrMsg(ilen+1);
        glGetProgramInfoLog(m_pid, ilen, NULL, pErrMsg.data());
        fprintf(stderr, "Failed to compile fragment shader:\n%s\n", pErrMsg.data());
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
        fprintf(stderr, "Failed to link shader program:\n%s\n", pErrMsg.data());
    }

    glDeleteShader(vs_id);
    glDeleteShader(fs_id);
}

void GLShader::use()
{
    glUseProgram(m_pid);
}
