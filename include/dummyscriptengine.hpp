#ifndef DUMMYSCRIPTENGINE_HPP
#define DUMMYSCRIPTENGINE_HPP

#include <scriptengine.hpp>

class DummyScriptEngine : public ScriptEngine
{
public:
    DummyScriptEngine();

    void init() override;
};

#endif // DUMMYSCRIPTENGINE_HPP
