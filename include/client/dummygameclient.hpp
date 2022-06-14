#ifndef DUMMYGAMECLIENT_HPP
#define DUMMYGAMECLIENT_HPP

#include "client/gameclient.hpp"

class DummyGameClient : public GameClient
{
public:
    DummyGameClient();

    void init() override;
    void run() override;
    void terminate() override;

    double getDeltaTime() const override;
    double getElapsedTime() const override;

    void lockCursor() override;
    void unlockCursor() override;
};

#endif // DUMMYGAMECLIENT_HPP
