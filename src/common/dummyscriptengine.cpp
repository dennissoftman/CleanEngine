#include <spdlog/spdlog.h>

#include "common/dummyscriptengine.hpp"
#include "common/servicelocator.hpp"

static const char *MODULE_NAME = "DummyScriptEngine";

DummyScriptEngine::DummyScriptEngine()
{

}

void DummyScriptEngine::init()
{
    spdlog::debug("Script engine init completed");
}
