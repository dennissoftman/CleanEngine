#include <steam/steam_api.h>
#include <spdlog/spdlog.h>

#include "client/steamgameservices.hpp"
#include "common/servicelocator.hpp"

static const char *MODULE_NAME = "SteamGameServices";

GameServices* GameServices::create()
{
    return new SteamGameServices();
}

SteamGameServices::SteamGameServices()
    : m_lastUpdateTime(0), m_lastCallbackTime(0)
{

}

SteamGameServices::~SteamGameServices()
{
    terminate();
}

void SteamGameServices::init()
{
    if(!SteamAPI_Init())
    {
        spdlog::error("Failed to init SteamAPI");
        return;
    }
}

void SteamGameServices::update(double dt)
{
    m_lastUpdateTime += dt;
    if(m_lastUpdateTime-m_lastCallbackTime >= 3.0) // 3 seconds
    {
        m_lastCallbackTime = m_lastUpdateTime;
        SteamAPI_RunCallbacks();
    }
}

void SteamGameServices::terminate()
{
    SteamAPI_Shutdown();
}

void SteamGameServices::authorize()
{

}
