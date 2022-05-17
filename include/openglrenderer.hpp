#ifndef OPENGLRENDERER_HPP
#define OPENGLRENDERER_HPP

#include "renderer.hpp"
#include <GL/glew.h>

#define OPENGL_RENDERER_TYPE "gl"

struct GLRenderObject
{
    explicit GLRenderObject(const Model3D *obj)
    {
        parent = obj;
        meshCount = obj->meshesCount;

        VAOs = new GLuint[meshCount];
        glGenVertexArrays(meshCount, VAOs);
        VBOs = new GLuint[meshCount];
        glGenBuffers(meshCount, VBOs);

        for(size_t i=0; i < meshCount; i++)
        {
            Mesh3D &mesh = obj->pMeshes[i];

            glBindVertexArray(VAOs[i]);
            glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);

            glBufferData(GL_ARRAY_BUFFER,
                         mesh.tris.size() * sizeof(Triangle3D),
                         mesh.tris.data(),
                         GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0,
                                  3,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  8*sizeof(GLfloat),
                                  (GLvoid*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1,
                                  2,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  8*sizeof(GLfloat),
                                  (GLvoid*)(3*sizeof(GLfloat)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2,
                                  3,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  8*sizeof(GLfloat),
                                  (GLvoid*)(5*sizeof(GLfloat)));
        }
        glBindVertexArray(0);
    }

    ~GLRenderObject()
    {
        if(meshCount > 0)
        {
            glDeleteBuffers(meshCount, VBOs);
            delete[] VBOs;
            glDeleteVertexArrays(meshCount, VAOs);
            delete[] VAOs;
        }
    }

    const Model3D *parent;

    GLuint *VAOs, *VBOs;
    size_t meshCount;
};

struct GLRenderRequest
{
    GLRenderObject *renderObject;
    glm::mat4 modelMatrix;
};

class GLMaterial;

class OpenGLRenderer : public Renderer
{
public:
    OpenGLRenderer();
    ~OpenGLRenderer() override;

    void init(const VideoMode &mode) override;

    void queueRenderObject(const Model3D *obj, const glm::mat4 &modelMatrix) override;
    void queueRenderObject(GLRenderObject *obj, const glm::mat4 &modelMatrix);
    void draw() override;

    [[nodiscard]] glm::ivec2 getSize() const override;
    void resize(const glm::ivec2 &size) override;

    void setProjectionMatrix(const glm::mat4 &projmx) override;
    void setViewMatrix(const glm::mat4 &viewmx) override;

    [[nodiscard]] std::string getType() const override;
private:
    GLRenderObject *createRenderObject(const Model3D *obj);

    glm::ivec2 m_currentSize;
    GLuint m_VAO, m_VBO;
    std::vector<GLRenderObject*> m_createdObjects;
    std::queue<GLRenderRequest> m_renderQueue;
    glm::mat4 m_projMatrix, m_viewMatrix;
    GLMaterial *m_defaultMaterial;

    bool m_was_init;
};

#endif // OPENGLRENDERER_HPP
