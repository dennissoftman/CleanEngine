#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <glm/glm.hpp>

class Shader
{
public:
    virtual void load(const std::string &vs, const std::string &fs) = 0;

    virtual void use() = 0;

    virtual void setMat4(const std::string_view &id, const glm::mat4 &mat) = 0;
};

#endif // SHADER_HPP
