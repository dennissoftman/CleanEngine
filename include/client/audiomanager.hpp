#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

struct SoundPropertiesInfo
{
    float volume=1.f;
    float pitch=1.f;
    glm::vec3 pos=glm::vec3(0,0,0);
    glm::vec3 vel=glm::vec3(0,0,0);
};

struct MusicPropertiesInfo
{
    float volume=1.f;
    float pitch=1.f;
    bool looping=true;
};

class AudioManager
{
public:
    virtual ~AudioManager() {}

    virtual void init() = 0;

    virtual void update(double dt=1/60.) = 0;

    virtual void updateListener(const glm::vec3 &pos,
                                const glm::vec3 &vel,
                                const glm::vec3 &fwd,
                                const glm::vec3 &up) = 0;

    virtual void loadSound(const std::string &path, const std::string &name) = 0;
    virtual void playSound(const std::string &name, const SoundPropertiesInfo &props={}) = 0;
    virtual void stopAllSounds() = 0;

    virtual void loadMusic(const std::string &path, const std::string &name) = 0;
    virtual void playMusic(const std::string &name, const MusicPropertiesInfo &props={}) = 0;
    virtual void stopAllMusic() = 0;
};

#endif // AUDIOMANAGER_HPP
