#include "audiomanager.hpp"
#include "servicelocator.hpp"

#include <filesystem>

static const char *MODULE_NAME = "AudioManager";

AudioManager::AudioManager()
    : m_audioSystem(nullptr)
{

}

AudioManager::~AudioManager()
{
    terminate();
}

void AudioManager::init()
{
    FMOD::System_Create(&m_audioSystem);
    m_audioSystem->init(2, FMOD_INIT_NORMAL, nullptr);
    {
        FMOD_VECTOR pos{}, vel{}, fwd{}, up{};
        fwd.z = 1;
        up.y = 1;
        m_audioSystem->set3DListenerAttributes(0, &pos, &vel, &fwd, &up);
    }
    ServiceLocator::getLogger().info(MODULE_NAME, "Init complete");
}

void AudioManager::terminate()
{
    if(m_audioSystem)
    {
        m_audioSystem->release();
        m_audioSystem = nullptr;
    }
}

void AudioManager::update(double dt)
{
    (void)dt;

    m_audioSystem->update();
}
