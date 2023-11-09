#ifndef UTILS_HPP
#define UTILS_HPP

#include <uuid/uuid_v4.h>

class Utils {
public:
    static UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
};

#endif // UTILS_HPP
