#ifndef GAMESERVICES_HPP
#define GAMESERVICES_HPP


class GameServices
{
public:
    virtual ~GameServices() {} // terminate services

    static GameServices *create();

    virtual void init() = 0; // init services
    virtual void update(double dt) = 0;
    virtual void authorize() = 0; // log in?
};

#endif // GAMESERVICES_HPP
