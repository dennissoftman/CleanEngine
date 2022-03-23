#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "shader.hpp"

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
    virtual void init(const VideoMode &mode) = 0;

    virtual void draw() = 0;
};

#endif // RENDERER_HPP
