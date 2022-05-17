#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "shader.hpp"
#include "material.hpp"
#include "modelmanager.hpp"

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

class Renderer
{
public:
    virtual ~Renderer() {}

    virtual void init(const VideoMode &mode) = 0;

    virtual void queueRenderObject(const Model3D *obj, const glm::mat4 &modelMatrix) = 0;
    virtual void draw() = 0;

    virtual glm::ivec2 getSize() const = 0;
    virtual void resize(const glm::ivec2 &size) = 0;

    virtual void setProjectionMatrix(const glm::mat4 &projmx) = 0;
    virtual void setViewMatrix(const glm::mat4 &viewmx) = 0;

    // lowercase renderer identifier (gl, vk, dx, etc.)
    virtual std::string getType() const = 0;
};

#endif // RENDERER_HPP
