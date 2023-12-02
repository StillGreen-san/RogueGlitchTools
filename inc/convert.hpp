#pragma once

#include "types.hpp"

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
inline UpgradeOptionFlags operator&(UpgradeOptionFlags lhs, UpgradeOptionFlags rhs)
{
	return static_cast<UpgradeOptionFlags>(static_cast<unsigned>(lhs) & static_cast<unsigned>(rhs));
}

DecryptedSave<Ultra> upgrade(DecryptedSave<Legacy> legacySave);
} // namespace rgt
