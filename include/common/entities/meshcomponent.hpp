#ifndef MESHCOMPONENT_HPP
#define MESHCOMPONENT_HPP

#include "common/entities/component.hpp"

class MeshComponent : public Component
{
public:
    MeshComponent(std::shared_ptr<Entity> parent);

    virtual void draw(Renderer *rend) = 0;
    virtual void update(double dt) = 0;

    virtual void show() = 0;
    virtual void hide() = 0;
    virtual bool isDrawable() const = 0;

    virtual void setModelId(uint32_t id) = 0;
    
    virtual void castShadow(bool cast) = 0;
    virtual bool isCastingShadow() const = 0;

    virtual void setTranslucent(bool translucent) = 0;
    virtual bool isTranslucent() const = 0;

    virtual const char *getName() const = 0;
protected:
    bool m_visible;
    bool m_castShadow;
    bool m_translucent;
};

#endif