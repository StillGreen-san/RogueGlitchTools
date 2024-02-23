#pragma once

#include "types.hpp"

#include <string>

namespace rgt
{
enum class UpgradeOptionFlags : unsigned
{
	None = 0,
	OldDefaults = 1,
};
inline UpgradeOptionFlags operator|(UpgradeOptionFlags lhs, UpgradeOptionFlags rhs)
{
	return static_cast<UpgradeOptionFlags>(static_cast<unsigned>(lhs) | static_cast<unsigned>(rhs));
}
inline bool operator&(UpgradeOptionFlags lhs, UpgradeOptionFlags rhs)
{
	return static_cast<bool>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
}

Version tryDetectVersion(const std::string& fileName, const std::string& content);

DecryptedSave<Ultra> upgrade(DecryptedSave<Legacy> legacySave, UpgradeOptionFlags options);
} // namespace rgt
