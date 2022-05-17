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

static const char *MODULE_NAME = "Main";

int main()
{
    ServiceLocator::init();
#ifndef NDEBUG
    FILE *debugFP = nullptr;
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
