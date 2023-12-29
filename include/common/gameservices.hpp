#ifndef GAMESERVICES_HPP
#define GAMESERVICES_HPP

#include <vector>
#include <string>

struct FriendDetails
{
    std::string name;
    // avatar?
    bool isOnline;
};

class GameServices
{
public:
    virtual ~GameServices() {} // terminate services

    static GameServices *create();

    virtual void init() = 0; // init services
    virtual void update(double dt) = 0;
    virtual void authorize() = 0; // log in?
    virtual std::vector<FriendDetails> getFriends() const = 0;
};

#endif // GAMESERVICES_HPP
