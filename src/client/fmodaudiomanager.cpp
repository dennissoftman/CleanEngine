#include "client/fmodaudiomanager.hpp"
#include "common/servicelocator.hpp"

#include <fstream>
#include <memory>
#include <sstream>

static const char *MODULE_NAME = "FmodAudioManager";

AudioManager *AudioManager::create()
{
    return new FmodAudioManager();
}

FmodAudioManager::FmodAudioManager()
    : m_audioSystem(nullptr),
      m_pos({}), m_vel({}),
      m_forward({}), m_up({})
{

}

FmodAudioManager::~FmodAudioManager()
{
    terminate();
}

void FmodAudioManager::init()
{
    FMOD::System_Create(&m_audioSystem);
    m_audioSystem->init(16, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, nullptr);
    {
        m_forward.z = -1;
        m_up.y = 1;
        m_audioSystem->set3DListenerAttributes(0, &m_pos, &m_vel, &m_forward, &m_up);
    }
    ServiceLocator::getLogger().info(MODULE_NAME, "Init completed");

    m_musicChannelsPool.resize(8, nullptr);
}

void FmodAudioManager::terminate()
{
    stopAllMusic();
    m_musicChannelsPool.clear();
    for(auto &kv : m_musicStreams)
        kv.second->release();
    m_musicResources.clear();

    stopAllSounds();
    for(auto &kv : m_preloadedSounds)
        kv.second->release();
    m_preloadedSounds.clear();

    m_audioSystem->release();
}

void FmodAudioManager::update(double dt)
{
    (void)dt;
    m_audioSystem->set3DListenerAttributes(0, &m_pos, &m_vel, &m_forward, &m_up);
    m_audioSystem->update();
    for(size_t i=0; i < m_musicChannelsPool.size(); i++)
    {
        if(m_musicChannelsPool[i] == nullptr)
            continue;
        bool active;
        m_musicChannelsPool[i]->isPlaying(&active);
        if(!active)
            m_musicChannelsPool[i] = nullptr;
    }
}

void FmodAudioManager::updateListener(const glm::vec3 &pos,
                                      const glm::vec3 &vel,
                                      const glm::vec3 &fwd,
                                      const glm::vec3 &up)
{
    m_pos = FMOD_VECTOR{pos.x, pos.y, pos.z};
    m_vel = FMOD_VECTOR{vel.x, vel.y, vel.z};
    m_forward = FMOD_VECTOR{fwd.x, fwd.y, fwd.z};
    m_up = FMOD_VECTOR{up.x, up.y, up.z};
}

void FmodAudioManager::loadSound(const std::string &path, const std::string &name)
{
    if(m_preloadedSounds.find(name) != m_preloadedSounds.end())
    {
        ServiceLocator::getLogger().error(MODULE_NAME, "Sound resource '"+name+"' already exists");
        return;
    }

    DataResource sndData = ServiceLocator::getResourceManager().getResource(path);

    FMOD::Sound *newSound = nullptr;
    FMOD_CREATESOUNDEXINFO createInfo{};
    createInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    createInfo.length = sndData.size;
    m_audioSystem->createSound(static_pointer_cast<const char>(sndData.data).get(),
                               FMOD_OPENMEMORY | FMOD_CREATESAMPLE | FMOD_3D,
                               &createInfo, &newSound);
    if(newSound)
        m_preloadedSounds[name] = newSound;
    else
    {
        std::stringstream errorstr;
        errorstr << "Failed to load sound '" << path << "'";
        ServiceLocator::getLogger().error(MODULE_NAME, errorstr.str());
    }
}

void FmodAudioManager::playSound(const std::string &name, const SoundPropertiesInfo &props)
{
    if(m_preloadedSounds.find(name) == m_preloadedSounds.end())
        return;
    FMOD::Sound *sound = m_preloadedSounds[name];
    FMOD::Channel *channel;
    m_audioSystem->playSound(sound, nullptr, false, &channel);
    channel->setVolume(props.volume);
    channel->setPitch(props.pitch);
    FMOD_VECTOR pos{props.pos.x, props.pos.y, props.pos.y}, vel{props.vel.x, props.vel.y, props.vel.z};
    channel->set3DAttributes(&pos, &vel);
}

void FmodAudioManager::stopAllSounds()
{
    // TODO
}

void FmodAudioManager::loadMusic(const std::string &path, const std::string &name)
{
    if(m_musicStreams.find(name) != m_musicStreams.end())
    {
        ServiceLocator::getLogger().error(MODULE_NAME, "Music resource '"+name+"' already exists");
        return;
    }

    DataResource musData = ServiceLocator::getResourceManager().getResource(path);
    m_musicResources.push_back(musData);

    FMOD::Sound *newMusic = nullptr;
    FMOD_CREATESOUNDEXINFO createInfo{};
    createInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    createInfo.length = musData.size;
    FMOD_RESULT r = m_audioSystem->createStream(static_pointer_cast<const char>(musData.data).get(),
                                                FMOD_OPENMEMORY | FMOD_CREATESTREAM | FMOD_2D,
                                                &createInfo, &newMusic);
    if(newMusic)
        m_musicStreams[name] = newMusic;
    else
    {
        std::stringstream errorstr;
        errorstr << "Failed to load music '" << path << "'. FMOD error: " << r;
        ServiceLocator::getLogger().error(MODULE_NAME, errorstr.str());
    }
}

void FmodAudioManager::playMusic(const std::string &name, const MusicPropertiesInfo &props)
{
    FMOD::Sound *music = m_musicStreams[name];

    FMOD::Channel *channel;
    m_audioSystem->playSound(music, nullptr, false, &channel);
    channel->setVolume(props.volume);
    channel->setPitch(props.pitch);

    for(size_t i=0; i < m_musicChannelsPool.size(); i++)
    {
        if(m_musicChannelsPool[i] == nullptr)
        {
            m_musicChannelsPool[i] = channel;
            return;
        }
    }
    size_t pos = m_musicChannelsPool.size();
    m_musicChannelsPool.reserve(m_musicChannelsPool.size() + 3);
    m_musicChannelsPool[pos] = channel;
}

void FmodAudioManager::stopAllMusic()
{
    for(auto &ch : m_musicChannelsPool)
    {
        if(ch == nullptr)
            continue;
        ch->stop();
    }
}

