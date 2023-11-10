#ifndef UTILS_HPP
#define UTILS_HPP

#include <stduuid/uuid.h>

class Utils {
public:
    static uuids::uuid_random_generator uuidGenerator;
    static std::mt19937 rndGenerator;
};

#endif // UTILS_HPP
