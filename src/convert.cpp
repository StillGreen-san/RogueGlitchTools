#include "convert.hpp"
#include "types.hpp"

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include <algorithm>
#include <array>
#include <string>
#include <string_view>

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
constexpr std::array<ReplacementInfo, 9> challengeReplacements{{
    {"Boxing Champ", "BoxingChamp"},                //
    {"Mountain of Corpses", "MountainOfCorpses"},   //
    {"No Fear!", "NoFear"},                         //
    {"Out of stock", "OutOfStock"},                 //
    {"Secret Room", "SecretRoom"},                  //
    {"Something to cool your anger...", "IceCube"}, //
    {"Teamwork Needed!", "TeamworkNeeded"},         //
    {"The Stomper!", "TheStomper"},                 //
    {"Bomb Specialist", "BombSpecialist"},          //
}};

auto find(nlohmann::json& json, std::string_view view)
{
	return std::find_if(json.begin(), json.end(),
	    [view](nlohmann::json& jsonItem)
	    {
		    return *jsonItem.get_ptr<std::string*>() == view;
	    });
}

template<typename TCollection>
void replace(nlohmann::json& json, std::string_view key, const TCollection& collection)
{
	auto& challengesItem = json[key]["value"];
	for(const auto& [oldThing, newThing] : collection)
	{
		auto oldVal = find(challengesItem, oldThing);
		if(oldVal != challengesItem.end())
		{
			*oldVal = newThing;
		}
	}
}
} // namespace

namespace rgt
{
DecryptedSave<Ultra> upgrade(
    DecryptedSave<Legacy> legacySave, UpgradeOptionFlags options) // NOLINT(*-value-param) resource is moved out
{
	nlohmann::json ultraJson = nlohmann::json::parse(ultraBaseSave);
	nlohmann::json legacyJson = nlohmann::json::parse(legacySave);

	for(const auto& legacyItem : legacyJson.items())
	{
		if(const auto ultraItem = ultraJson.find(legacyItem.key()); ultraItem != ultraJson.end())
		{
			ultraItem.value()["value"] = legacyItem.value()["value"];
		}
	}

	replace(ultraJson, "UnlockedItems_V2", itemReplacements);
	replace(ultraJson, "ChallengesDone_V2", challengeReplacements);

	return {ultraJson.dump()};
}
} // namespace rgt
