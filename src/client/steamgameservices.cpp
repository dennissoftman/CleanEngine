#include <steam/steam_api.h>
#include <spdlog/spdlog.h>

#include "client/steamgameservices.hpp"
#include "common/servicelocator.hpp"

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
    SteamErrMsg msg;
    if(SteamAPI_InitEx(&msg) != k_ESteamAPIInitResult_OK)
    {
        spdlog::error(fmt::format("Failed to init SteamAPI: {}", msg));
        return;
    }
    spdlog::debug("SteamAPI init OK");
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
    spdlog::debug("SteamAPI terminate OK");
}

void SteamGameServices::authorize()
{

}

std::vector<FriendDetails> SteamGameServices::getFriends() const
{
    std::vector<FriendDetails> friends;
    int numFriends = SteamFriends()->GetFriendCount(k_EFriendFlagImmediate);
    for(int i = 0; i < numFriends; ++i)
    {
        CSteamID friendID = SteamFriends()->GetFriendByIndex(i, k_EFriendFlagImmediate);
        FriendDetails details;
        details.name = SteamFriends()->GetFriendPersonaName(friendID);
        details.isOnline = SteamFriends()->GetFriendPersonaState(friendID) == k_EPersonaStateOnline;
        friends.push_back(details);
    }
    return friends;
}
