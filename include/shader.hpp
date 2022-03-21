#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>

class Shader
{
public:
    virtual void load(const std::string &vs, const std::string &fs) = 0;

    virtual void use() = 0;
};

#endif // SHADER_HPP
