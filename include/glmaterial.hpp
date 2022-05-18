#ifndef GLMATERIAL_HPP
#define GLMATERIAL_HPP

#include "material.hpp"
#include "openglrenderer.hpp"
#include "glshader.hpp"

#define GL_MAT_PROJ_MX "projMatrix"
#define GL_MAT_VIEW_MX "viewMatrix"
#define GL_MAT_MODEL_MX "modelMatrix"

#include <unordered_map>

class GLMaterial : public Material
{
public:
    explicit GLMaterial(OpenGLRenderer *rend);
    ~GLMaterial();

    void init() override;

    void setImage(const std::string &path, const std::string &name) override;

    void use(TransformData &data) override;

    void setDoubleSided(bool yes) override;
    bool isDoubleSided() const override;
private:
    static GLuint loadTexture(const std::string &path);

    OpenGLRenderer *m_renderer;
    GLShader *m_shader;

    //
    bool m_doubleSided;

    std::unordered_map<std::string, GLuint> m_textures;
    //
};

#endif // GLMATERIAL_HPP
