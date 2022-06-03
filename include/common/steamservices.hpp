#ifndef STEAMSERVICES_HPP
#define STEAMSERVICES_HPP

#include "common/gameservices.hpp"

class SteamServices : public GameServices
{
public:
    SteamServices();
    ~SteamServices() override;

    void init() override;
    void terminate();
    void authorize() override;

private:

};

#endif // STEAMSERVICES_HPP
