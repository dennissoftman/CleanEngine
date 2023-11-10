#include <string>
#include <stdexcept>
#include <cstdio>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "common/servicelocator.hpp"
#include "client/gamefrontend.hpp"
#include "server/gamebackend.hpp"

int main()
{
    // init logging
    spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] %v");
#ifndef NDEBUG
    std::shared_ptr<spdlog::logger> logger; // must be in highest main() context not to be destroyed
    try
    {
        spdlog::set_level(spdlog::level::debug);
        logger = spdlog::basic_logger_mt(APP_NAME, "debug.log");
        spdlog::set_default_logger(logger);
    }
    catch (const std::exception &e)
    {
        spdlog::error("Failed to init file logger");
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
    // init physics
    ServiceLocator::getPhysicsManager().init();
    // init audio
    ServiceLocator::getAudioManager().init();
    // init game services
    ServiceLocator::getGameServices().init();
    // init game services
    ServiceLocator::getGameServices().init();
    // init network manager
    ServiceLocator::getGameServer().init();
*/
//    auto backend = std::make_unique<GameBackend>(GameBackend::create());
//    backend->init();

    auto frontend = std::unique_ptr<GameFrontend>(GameFrontend::create());
    frontend->init();

    // backend->run(); // run in background
    frontend->run(); // run in foreground

    return 0;
}
