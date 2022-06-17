#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "client/shader.hpp"
#include "client/material.hpp"
#include "common/modelmanager.hpp"

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

class Camera3D;

class Renderer
{
public:
    static Renderer *create();
    virtual ~Renderer() {}

    virtual void init(const VideoMode &mode) = 0;

    virtual void queueRenderObject(const Model3D *obj, const glm::mat4 &modelMatrix) = 0;
    virtual void draw() = 0;

    virtual glm::ivec2 getSize() const = 0;
    virtual void resize(const glm::ivec2 &size) = 0;

    virtual void updateCameraData(Camera3D &cam) = 0;
    virtual void updateLightPosition(const glm::vec4 &pos, int id=0) = 0;
    virtual void updateLightColor(const glm::vec4 &color, int id=0) = 0;

    // lowercase renderer identifier (gl, vk, dx, etc.)
    virtual std::string getType() const = 0;

    static const int MaxLightSourceCount = 16;
};

#endif // RENDERER_HPP
