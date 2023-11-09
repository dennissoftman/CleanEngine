#ifndef DUMMYSERVICES_HPP
#define DUMMYSERVICES_HPP

#include "common/gameservices.hpp"

class DummyGameServices : public GameServices
{
public:
    DummyGameServices();
    ~DummyGameServices() override;

    void init() override;
    void update(double dt) override;
    void authorize() override;
};

#endif // DUMMYSERVICES_HPP
