#ifndef STEAMSERVICES_HPP
#define STEAMSERVICES_HPP

#include "common/gameservices.hpp"

class SteamGameServices : public GameServices
{
public:
    SteamGameServices();
    ~SteamGameServices() override;

    void init() override;
    void update(double dt) override;
    void terminate();
    void authorize() override;

private:
    double m_lastUpdateTime, m_lastCallbackTime;
};

#endif // STEAMSERVICES_HPP
