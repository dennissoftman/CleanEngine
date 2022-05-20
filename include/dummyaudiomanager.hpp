#ifndef DUMMYAUDIOMANAGER_HPP
#define DUMMYAUDIOMANAGER_HPP

#include <audiomanager.hpp>

class DummyAudioManager : public AudioManager
{
public:
    DummyAudioManager();

    void init() override;

    void update(double dt) override;

    void updateListener(const glm::vec3 &pos,
                        const glm::vec3 &vel,
                        const glm::vec3 &fwd,
                        const glm::vec3 &up) override;

    void loadSound(const std::string &path, const std::string &name) override;
    void playSound(const std::string &name, const SoundPropertiesInfo &props) override;
    void stopAllSounds() override;

    void loadMusic(const std::string &path, const std::string &name) override;
    void playMusic(const std::string &name, const MusicPropertiesInfo &props) override;
    void stopAllMusic() override;
};

#endif // DUMMYAUDIOMANAGER_HPP
