#include "client/openalaudiomanager.hpp"
#include <stdexcept>

OpenALAudioManager::OpenALAudioManager()
    : m_device(nullptr), m_context(nullptr)
{

}

OpenALAudioManager::~OpenALAudioManager()
{
    terminate();
}

void OpenALAudioManager::init()
{
    m_device = alcOpenDevice(NULL);
    if(m_device == nullptr)
    {
        throw std::runtime_error("failed to init openal");
    }
    m_context = alcCreateContext(m_device, nullptr);
}

void OpenALAudioManager::terminate()
{
    if(m_context)
    {
        alcDestroyContext(m_context);
        m_context = nullptr;
    }
    if(m_device)
    {
        alcCloseDevice(m_device);
        m_device = nullptr;
    }
}

void OpenALAudioManager::update(double dt)
{

}

void OpenALAudioManager::updateListener(const glm::vec3 &pos,
                                        const glm::vec3 &vel,
                                        const glm::vec3 &fwd,
                                        const glm::vec3 &up)
{

}

void OpenALAudioManager::loadSound(const std::string &path, const std::string &name)
{

}

void OpenALAudioManager::playSound(const std::string &name, const SoundPropertiesInfo &props)
{

}

void OpenALAudioManager::stopAllSounds()
{

}

void OpenALAudioManager::loadMusic(const std::string &path, const std::string &name)
{

}

void OpenALAudioManager::playMusic(const std::string &name, const MusicPropertiesInfo &props)
{

}

void OpenALAudioManager::stopAllMusic()
{

}
