#ifndef FMODAUDIOMANAGER_HPP
#define FMODAUDIOMANAGER_HPP

#include "client/audiomanager.hpp"
#include "common/resourcemanager.hpp"

#include <fmod.hpp>
#include <vector>

class FmodAudioManager : public AudioManager
{
public:
    FmodAudioManager();
    ~FmodAudioManager() override;

    void init() override;
    void terminate();

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
private:
    FMOD::System *m_audioSystem;

    std::unordered_map<std::string, FMOD::Sound*> m_preloadedSounds;
    std::unordered_map<std::string, FMOD::Sound*> m_musicStreams;
    std::vector<std::shared_ptr<DataResource>> m_musicResources;

    std::vector<FMOD::Channel*> m_musicChannelsPool;

    FMOD_VECTOR m_pos, m_vel, m_forward, m_up;
};

#endif // FMODAUDIOMANAGER_HPP
