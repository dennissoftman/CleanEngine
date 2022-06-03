#include "common/steamservices.hpp"
#include "common/servicelocator.hpp"

#include <steam/steam_api.h>

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
        ServiceLocator::getLogger().error(MODULE_NAME, "Failed to init SteamAPI");
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
