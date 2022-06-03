#ifndef GAMESERVICES_HPP
#define GAMESERVICES_HPP


class GameServices
{
public:
    virtual ~GameServices() {} // terminate services

    virtual void init() = 0; // init services

    virtual void authorize() = 0; // log in?
};

#endif // GAMESERVICES_HPP
