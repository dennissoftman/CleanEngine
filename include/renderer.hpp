#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "shader.hpp"
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

struct Triangle2D
{
    glm::vec2 v0; glm::vec3 c0;
    glm::vec2 v1; glm::vec3 c1;
    glm::vec2 v2; glm::vec3 c2;
};

struct Model2D
{
    Model2D() {}
    Model2D(const std::vector<Triangle2D> &t)
        : tris(t)
    { }
    std::vector<Triangle2D> tris;
};

struct RenderObject
{
    Model2D *model;
    Shader *shader;

    glm::mat4 modelMatrix;
    const void *pUserData;
};

class Renderer
{
public:
    virtual void init(const VideoMode &mode) = 0;

    virtual void queueRenderObject(RenderObject *obj) = 0;
    virtual void draw() = 0;
};

#endif // RENDERER_HPP
