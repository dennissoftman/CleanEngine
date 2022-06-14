#ifndef SCRIPTENGINE_HPP
#define SCRIPTENGINE_HPP

#include <string>

class ScriptEngine
{
public:
    static ScriptEngine *create();

    // All the functions
    virtual void init() = 0;

};

#endif // SCRIPTENGINE_HPP
