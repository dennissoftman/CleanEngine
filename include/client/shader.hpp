#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <glm/glm.hpp>

enum ShaderType
{
    ShaderVertex=0x1,
    ShaderFragment=0x2,
    ShaderGeometry=0x4
};

class Shader
{
public:
    // load from memory
    virtual void load(const char *vdata, size_t vsize, const char *fdata, size_t fsize) = 0;

    // load from file
    virtual void load(const std::string &vs, const std::string &fs) = 0;

    virtual void use() = 0;

    virtual void setMat4(const std::string_view &id, const glm::mat4 &mat) = 0;

    virtual void setInt(const std::string_view &id, int a) = 0;
};

#endif // SHADER_HPP
