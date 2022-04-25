#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "shader.hpp"
#include "material.hpp"

#include <queue>
#include <glm/glm.hpp>

struct VideoMode
{
    VideoMode(int w, int h, bool f=false)
        : width(w), height(h), fullscreen(f)
    {

    }
    int width, height;
    bool fullscreen;
};

// vertCoord, texCoord [, normCoord]
struct Triangle3D
{
    glm::vec3 v0; glm::vec2 t0;
    glm::vec3 v1; glm::vec2 t1;
    glm::vec3 v2; glm::vec2 t2;
};

struct Model3D
{
    Model3D() {}
    Model3D(const std::vector<Triangle3D> &t)
        : tris(t)
    { }
    std::vector<Triangle3D> tris;
};

struct RenderObject
{
    const Model3D *model;
    Material *mat;

    glm::mat4 modelMatrix;
    const void *pUserData;
};

class Renderer
{
public:
    virtual void init(const VideoMode &mode) = 0;

    virtual void queueRenderObject(RenderObject *obj) = 0;
    virtual void draw() = 0;

    virtual void setProjectionMatrix(const glm::mat4 &projmx) = 0;
    virtual void setViewMatrix(const glm::mat4 &viewmx) = 0;
};

#endif // RENDERER_HPP
