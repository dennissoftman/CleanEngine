#ifndef OPENALAUDIOMANAGER_HPP
#define OPENALAUDIOMANAGER_HPP

#include "client/audiomanager.hpp"

#include <AL/al.h>
#include <AL/alc.h>

class OpenALAudioManager : public AudioManager
{
public:
    OpenALAudioManager();
    ~OpenALAudioManager();

    void init() override;
    void terminate();

    void update(double dt) override;

    void updateListener(const glm::vec3 &pos, const glm::vec3 &vel, const glm::vec3 &fwd, const glm::vec3 &up) override;

    void loadSound(const std::string &path, const std::string &name) override;
    void playSound(const std::string &name, const SoundPropertiesInfo &props) override;
    void stopAllSounds() override;

    void loadMusic(const std::string &path, const std::string &name) override;
    void playMusic(const std::string &name, const MusicPropertiesInfo &props) override;
    void stopAllMusic() override;
private:
    ALCdevice *m_device;
    ALCcontext *m_context;


};

#endif // OPENALAUDIOMANAGER_HPP
