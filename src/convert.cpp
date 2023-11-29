#include "convert.hpp"
#include "types.hpp"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

namespace
{
struct ReplacementInfo
{
	std::string_view oldThing;
	std::string_view newThing;
};

constexpr std::array<ReplacementInfo, 6> replacements{{
    {"System.Int32,mscorlib", "int"},      //
    {"System.Boolean,mscorlib", "bool"},   //
    {"DarkMissiles", "ViralInfection"},    //
    {"FireRateUpOnCrit", "CritRing"},      //
    {"SuperJumpBoots", "BoostedJumps"},    //
    {"Version=2.0.0.0", "Version=4.0.0.0"} //
}};
} // namespace

namespace rgt
{
DecryptedSave<Ultra> upgrade(DecryptedSave<Legacy> save)
{
	for(const auto& [oldThing, newThing] : replacements)
	{
		size_t offset = 0;
		while(true)
		{
			offset = save.find(oldThing, offset);
			if(offset == std::string::npos)
			{
				break;
			}
			save.replace(offset, oldThing.size(), newThing);
		}
	}
	return {std::move(save)};
}
} // namespace rgt
