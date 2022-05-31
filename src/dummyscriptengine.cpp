#include "dummyscriptengine.hpp"
#include "servicelocator.hpp"

static const char *MODULE_NAME = "DummyScriptEngine";

DummyScriptEngine::DummyScriptEngine()
{

}

void DummyScriptEngine::init()
{
    ServiceLocator::getLogger().info(MODULE_NAME, "Script engine init completed");
}
