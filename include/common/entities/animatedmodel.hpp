#ifndef ANIMATED_MODEL_HPP
#define ANIMATED_MODEL_HPP

#include "common/entities/staticmesh.hpp"
#include "common/modelmanager.hpp"


class AnimatedModelComponent : public StaticMesh
{
public:
    AnimatedModelComponent(std::shared_ptr<Entity> parent);
    ~AnimatedModelComponent() override;

    void draw(Renderer *rend) override;
    void update(double dt) override;

    void setAnimation(std::shared_ptr<AnimationPrimitive> anim);

    // trigger animation
    // returns true on success
    bool trigger();

    void setLooping(bool yes);

    double time() const;

    const char *getName() const override;
private:
    double m_time;
    bool m_looping;
    bool m_shouldPlay;
    std::shared_ptr<AnimationPrimitive> m_currentAnimation; // TODO: add animation list
    std::vector<glm::mat4> m_meshTransforms;
};

#endif