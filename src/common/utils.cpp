#include "common/utils.hpp"

std::mt19937 Utils::rndGenerator = std::mt19937();
uuids::uuid_random_generator Utils::uuidGenerator = uuids::uuid_random_generator(&Utils::rndGenerator);
