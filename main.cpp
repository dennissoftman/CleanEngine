#include <string>
#include <stdexcept>
#include <cstdio>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "common/servicelocator.hpp"
#include "client/gamefrontend.hpp"
#include "server/gamebackend.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
    // init logging
    spdlog::set_pattern("[%H:%M:%S %z] [%^---%L---%$] %v");
#ifndef NDEBUG
    try
    {
        spdlog::set_level(spdlog::level::debug);
    }
    catch (const std::exception &e)
    {
        spdlog::error(fmt::format("Failed to init file logger: {}", e.what()));
    }
#else // release
    {
        spdlog::set_level(spdlog::level::warn);
    }
#endif

    ServiceLocator::init();
    std::atexit(ServiceLocator::terminate);

    ServiceLocator::getResourceManager().init(); // init configs
/*
    // init network manager
    ServiceLocator::getGameServer().init();
*/
    // init physics
    ServiceLocator::getPhysicsManager().init();
    // init audio
    ServiceLocator::getAudioManager().init();
    // init game services
    ServiceLocator::getGameServices().init();
//    auto backend = std::make_unique<GameBackend>(GameBackend::create());
//    backend->init();

    auto frontend = std::unique_ptr<GameFrontend>(GameFrontend::create());
    frontend->init();

    // backend->run(); // run in background
    frontend->run(); // run in foreground

    return 0;
}
