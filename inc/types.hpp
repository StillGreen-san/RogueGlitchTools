#pragma once

#include <cstdint>
#include <string>

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
class EncryptedSave : public std::string
{
};
} // namespace rgt
