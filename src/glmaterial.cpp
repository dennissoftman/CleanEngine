#include "glmaterial.hpp"
#include "glshader.hpp"
#include "servicelocator.hpp"

#include <IL/il.h>
#include <IL/ilu.h>

static const char *MODULE_NAME = "GLMaterial";

GLMaterial::GLMaterial()
{

}

GLMaterial::~GLMaterial()
{

}

GLuint GLMaterial::loadTexture(const std::string &path)
{
    ILuint imgId = ilGenImage();
    ilBindImage(imgId);

    GLuint id;
    if(!ilLoadImage(path.c_str()))
    {
        ServiceLocator::getLogger().error(MODULE_NAME, "Image loading error");
        ilBindImage(0);
        ilDeleteImage(imgId);
        return 0;
    }

    iluFlipImage(); // flip for OpenGL

    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
                 ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),
                 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());

    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1);

    float *maxA = (float*)calloc(1, sizeof(float));
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, maxA);
    float a_amount = std::min(4.f, *maxA);
    if(maxA != nullptr)
        free(maxA);
    glTextureParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, a_amount);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    ilBindImage(0);
    ilDeleteImage(imgId);

    return id;
}

void GLMaterial::init()
{
    std::string vertexShader = R"(
#version 450 core
layout(location = 0) in vec3 vertCoord;
layout(location = 1) in vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec2 texPos;

void main()
{
    texPos = texCoord;
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(vertCoord, 1.0);
}
)";
    std::string fragmentShader = R"(
#version 450 core
out vec4 fragColor;

uniform sampler2D img;

in vec2 texPos;

void main()
{
    fragColor = texture(img, texPos);
}
)";

    m_shader.load(vertexShader.data(), vertexShader.size(),
                  fragmentShader.data(), fragmentShader.size());

    //
}

void GLMaterial::setImage(const std::string &name, const std::string &path)
{
    m_textures[name] = loadTexture(path);
}

void GLMaterial::use(const TransformData &data)
{
    m_shader.use();
    // bind textures, set colors, set matrices
    m_shader.setMat4(GL_MAT_PROJ_MX, data.Projection);
    m_shader.setMat4(GL_MAT_VIEW_MX, data.View);
    m_shader.setMat4(GL_MAT_MODEL_MX, data.Model);

    int i=0;
    for(auto &kv : m_textures)
    {
        m_shader.setInt(kv.first, i);
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D, kv.second);

        i++;
    }

    if(m_doubleSided)
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);
}

void GLMaterial::setDoubleSided(bool yes)
{
    m_doubleSided = yes;
}

bool GLMaterial::isDoubleSided() const
{
    return m_doubleSided;
}
