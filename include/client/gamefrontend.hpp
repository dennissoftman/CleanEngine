#ifndef GAMECLIENT_HPP
#define GAMECLIENT_HPP

#include <functional>

class Scene3D;
class GameFrontend
{
public:
    virtual ~GameFrontend() {}

    static GameFrontend *create();

    virtual void init() = 0;
    virtual void run() = 0;
    virtual void terminate() = 0;

    virtual double getDeltaTime() const = 0;
    virtual double getElapsedTime() const = 0;

    virtual void lockCursor() = 0;
    virtual void unlockCursor() = 0;

    virtual void updateSubscribe(const std::function<void (double)> &callb) = 0;

    static GameFrontend *corePtr;
};

#endif // GAMECLIENT_HPP
