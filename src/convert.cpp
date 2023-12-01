#include "convert.hpp"
#include "types.hpp"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

namespace
{
constexpr std::string_view ultraBaseSave = R"({
	"WinStreak" : { "__type" : "int", "value" : 0 },
	"UnlockedMaxWorld": { "__type": "int", "value": 1 },
	"UpgradeCurrency": { "__type": "int", "value": 0 },
	"UnlockedSkins" : { "__type" : "System.Collections.Generic.List`1[[System.String, mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]],mscorlib",
		"value" : [ "Default" ] },
	"UnlockedChars_V2" : { "__type" : "System.Collections.Generic.List`1[[System.String, mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]],mscorlib",
		"value" : [ "Chap" ] },
	"UnlockedItems_V2" : { "__type" : "System.Collections.Generic.List`1[[System.String, mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]],mscorlib",
		"value" : [ ] },
	"ChallengesDone_V2": { "__type": "System.Collections.Generic.List`1[[System.String, mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089]],mscorlib",
		"value": [ ] },
	"BabyLevel_V2": { "__type": "int", "value": 0 },
	"BobLevel_V2": { "__type": "int", "value": 0 },
	"ChapLevel_V2": { "__type": "int", "value": 0 },
	"CrocLevel_V2": { "__type": "int", "value": 0 },
	"DullyLevel_V2": { "__type": "int", "value": 0 },
	"FailyLevel_V2": { "__type": "int", "value": 0 },
	"GlitchPixelLevel_V2": { "__type": "int", "value": 0 },
	"IgorLevel_V2": { "__type": "int", "value": 0 },
	"LunaLevel_V2": { "__type": "int", "value": 0 },
	"QuickLevel_V2": { "__type": "int", "value": 0 },
	"SpiderLevel_V2": { "__type": "int", "value": 0 },
	"SquibLevel_V2": { "__type": "int", "value": 0 }
})";

struct ReplacementInfo
{
	std::string_view oldThing;
	std::string_view newThing;
};

constexpr std::array<ReplacementInfo, 3> itemReplacements{{
    {"DarkMissiles", "ViralInfection"}, //
    {"FireRateUpOnCrit", "CritRing"},   //
    {"SuperJumpBoots", "BoostedJumps"}, //
}};
constexpr std::array<ReplacementInfo, 8> challengeRplacements{{
    {"Boxing Champ", "BoxingChamp"},                //
    {"Mountain of Corpses", "MountainOfCorpses"},   //
    {"No Fear!", "NoFear"},                         //
    {"Out of stock", "OutOfStock"},                 //
    {"Secret Room", "SecretRoom"},                  //
    {"Something to cool your anger...", "IceCube"}, //
    {"Teamwork Needed!", "TeamworkNeeded"},         //
    {"The Stomper!", "TheStomper"},                 //
}};

constexpr std::array<ReplacementInfo, 14> replacements{{
    {"System.Int32,mscorlib", "int"},               //
    {"System.Boolean,mscorlib", "bool"},            //
    {"DarkMissiles", "ViralInfection"},             //
    {"FireRateUpOnCrit", "CritRing"},               //
    {"SuperJumpBoots", "BoostedJumps"},             //
    {"Boxing Champ", "BoxingChamp"},                //
    {"Mountain of Corpses", "MountainOfCorpses"},   //
    {"No Fear!", "NoFear"},                         //
    {"Out of stock", "OutOfStock"},                 //
    {"Secret Room", "SecretRoom"},                  //
    {"Something to cool your anger...", "IceCube"}, //
    {"Teamwork Needed!", "TeamworkNeeded"},         //
    {"The Stomper!", "TheStomper"},                 //
    {"Version=2.0.0.0", "Version=4.0.0.0"}          //
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
