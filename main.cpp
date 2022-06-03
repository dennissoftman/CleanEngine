#include <string>
#include <stdexcept>
#include <cstdio>

#include "common/servicelocator.hpp"
#include "common/debuglogger.hpp"

#ifdef CLIENT_GLFW
#include "client/gameclientglfw.hpp"
#else
#error No core library selected
#endif

#ifdef PHYSICS_BULLET
#include "server/bulletphysicsmanager.hpp"
#endif

#ifdef AUDIO_FMOD
#include "client/fmodaudiomanager.hpp"
#endif

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

#ifdef AUDIO_FMOD
    {
        FmodAudioManager *fmodAudioManager = new FmodAudioManager();
        fmodAudioManager->init();
        ServiceLocator::setAudioManager(fmodAudioManager);
    }
#endif

    ServiceLocator::getResourceManager().init(); // init configs

#ifdef SERVICES_STEAM
    {
        SteamServices *steamServices = new SteamServices();
        steamServices->init();
        ServiceLocator::setGameServices(steamServices);
    }
#endif

#ifdef CLIENT_GLFW
    GameClientGLFW client{};

    client.init();
    client.mainLoop();
#else
#error No client selected
#endif
    client.terminate();

#ifndef NDEBUG
    fclose(debugFP);
#endif

    return 0;
}
