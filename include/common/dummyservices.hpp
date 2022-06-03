#ifndef DUMMYSERVICES_HPP
#define DUMMYSERVICES_HPP

#include "common/gameservices.hpp"

class DummyServices : public GameServices
{
public:
    DummyServices();
    ~DummyServices() override;

    void init() override;
    void authorize() override;
};

#endif // DUMMYSERVICES_HPP
