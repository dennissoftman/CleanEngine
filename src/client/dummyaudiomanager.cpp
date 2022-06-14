#include "client/dummyaudiomanager.hpp"

AudioManager *AudioManager::create()
{
    return new DummyAudioManager();
}

DummyAudioManager::DummyAudioManager()
{

}

void DummyAudioManager::init()
{

}

void DummyAudioManager::update(double dt)
{
    (void)dt;
}

void DummyAudioManager::updateListener(const glm::vec3 &pos, const glm::vec3 &vel, const glm::vec3 &fwd, const glm::vec3 &up)
{
    (void)pos;
    (void)vel;
    (void)fwd;
    (void)up;
}

void DummyAudioManager::loadSound(const std::string &path, const std::string &name)
{
    (void)path;
    (void)name;
}

void DummyAudioManager::playSound(const std::string &name, const SoundPropertiesInfo &props)
{
    (void)name;
    (void)props;
}

void DummyAudioManager::stopAllSounds()
{

}

void DummyAudioManager::loadMusic(const std::string &path, const std::string &name)
{
    (void)path;
    (void)name;
}

void DummyAudioManager::playMusic(const std::string &name, const MusicPropertiesInfo &props)
{
    (void)name;
}

void DummyAudioManager::stopAllMusic()
{

}
