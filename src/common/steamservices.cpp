#include <steam/steam_api.h>
#include <spdlog/spdlog.h>

#include "common/steamservices.hpp"
#include "common/servicelocator.hpp"

static const char *MODULE_NAME = "SteamServices";

SteamServices::SteamServices()
{

}

SteamServices::~SteamServices()
{
    terminate();
}

void SteamServices::init()
{
    if(!SteamAPI_Init())
    {
        spdlog::error("Failed to init SteamAPI");
        return;
    }
}

void SteamServices::terminate()
{
    SteamAPI_Shutdown();
}

void SteamServices::authorize()
{

}
