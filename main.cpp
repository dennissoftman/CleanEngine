#include <string>
#include <stdexcept>
#include <cstdio>

#include "servicelocator.hpp"

#ifndef NDEBUG
#include "debuglogger.hpp"
#endif

#ifdef CORE_GLFW
#include "enginecoreglfw.hpp"
#else
#error No core library selected
#endif

const std::string MODULE_NAME = "Main";

int main()
{
    FILE *debugFP = NULL;
    ServiceLocator::init();
#ifndef NDEBUG
    {
       debugFP = fopen("debug.log", "a");
       DebugLogger *logger = new DebugLogger();
       logger->setInfoFP(debugFP);
       logger->setWarningFP(debugFP);
       logger->setErrorFP(debugFP);
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

    if(debugFP)
        fclose(debugFP);

    return 0;
}
