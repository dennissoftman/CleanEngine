#include <string>
#include <stdexcept>
#include <cstdio>

#include "servicelocator.hpp"

#include "debuglogger.hpp"

#ifdef CORE_GLFW
#include "enginecoreglfw.hpp"
#else
#error No core library selected
#endif

#ifdef PHYSICS_BULLET
#include "bulletphysicsmanager.hpp"
#endif

#ifdef AUDIO_FMOD
#include "fmodaudiomanager.hpp"
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
    }
#else // release
    {
        DebugLogger *logger = new DebugLogger();
        logger->addErrorFP(stderr);
        ServiceLocator::setLogger(logger);
    }
#endif
    ServiceLocator::getLogger().info(MODULE_NAME, "Started logging");

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

// TODO: create EngineCore parent class to remove macros from main.cpp
#ifdef CORE_GLFW
    EngineCoreGLFW core{};

    core.init();
    core.mainLoop();
#else
#error No core library selected
#endif
    core.terminate();

#ifndef NDEBUG
    fclose(debugFP);
#endif

    return 0;
}
