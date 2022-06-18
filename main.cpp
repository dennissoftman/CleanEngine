#include <string>
#include <stdexcept>
#include <cstdio>

#include "common/servicelocator.hpp"
#include "common/debuglogger.hpp"

#include "client/gamefrontend.hpp"

#ifdef PHYSICS_BULLET
#include "server/bulletphysicsmanager.hpp"
#endif

#include "client/audiomanager.hpp"

#ifdef SERVICES_STEAM
#include "common/steamservices.hpp"
#endif

static const char *MODULE_NAME = "Main";

int main()
{
    ServiceLocator::init(); // AT FIRST!
#ifndef NDEBUG
    FILE *debugFP = NULL;
    {
        debugFP = fopen("debug.log", "a");
        DebugLogger *logger = new DebugLogger();
        logger->addInfoFP(stdout);
        logger->addInfoFP(debugFP);
        logger->addWarningFP(stdout);
        logger->addWarningFP(debugFP);
        logger->addErrorFP(stderr);
        logger->addErrorFP(debugFP);
        ServiceLocator::setLogger(logger);

        ServiceLocator::getLogger().info(MODULE_NAME, "Started logging");
    }
#else // release
    {
        DebugLogger *logger = new DebugLogger();
        logger->addErrorFP(stderr);
        ServiceLocator::setLogger(logger);
    }
#endif

#ifdef PHYSICS_BULLET
    {
        BulletPhysicsManager *bulletPhysicsManager = new BulletPhysicsManager();
        bulletPhysicsManager->init();
        ServiceLocator::setPhysicsManager(bulletPhysicsManager);
    }
#endif

    { // Audio manager
        AudioManager *audmgr = AudioManager::create();
        audmgr->init();
        ServiceLocator::setAudioManager(audmgr);
    }

    ServiceLocator::getResourceManager().init(); // init configs

#ifdef SERVICES_STEAM
    {
        SteamServices *steamServices = new SteamServices();
        steamServices->init();
        ServiceLocator::setGameServices(steamServices);
    }
#endif

    { // game client
        GameFrontend *client = GameFrontend::create();
        client->init();
        client->run(); // blocking
        delete client;
    }

#ifndef NDEBUG
    fclose(debugFP);
#endif

    return 0;
}
