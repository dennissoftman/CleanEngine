#include "common/utils.hpp"

UUIDv4::UUIDGenerator<std::mt19937_64> Utils::uuidGenerator = UUIDv4::UUIDGenerator<std::mt19937_64>();
