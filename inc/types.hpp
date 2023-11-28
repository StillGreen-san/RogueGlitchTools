#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace rgt
{
enum Version : std::uint8_t
{
	Legacy,
	Ultra,
};

template<Version TVersion>
class DecryptedSave : public std::string
{
};
template<Version TVersion>
class EncryptedSave : public std::vector<unsigned char>
{
};
} // namespace rgt
