#ifndef GLMATERIAL_HPP
#define GLMATERIAL_HPP

#include "material.hpp"
#include "glshader.hpp"

#define GL_MAT_PROJ_MX "projMatrix"
#define GL_MAT_VIEW_MX "viewMatrix"
#define GL_MAT_MODEL_MX "modelMatrix"

#include <unordered_map>

class GLMaterial : public Material
{
public:
    GLMaterial();
    ~GLMaterial();

    void init() override;

    void setImage(const std::string &name, const std::string &path) override;

    void use(const TransformData &data) override;

    void setDoubleSided(bool yes) override;
    bool isDoubleSided() const override;
private:
    static GLuint loadTexture(const std::string &path);

    GLShader m_shader;

    //
    bool m_doubleSided;

    std::unordered_map<std::string, GLuint> m_textures;
    //
};

#endif // GLMATERIAL_HPP
