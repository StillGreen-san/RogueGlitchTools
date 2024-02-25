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

constexpr std::string_view valueKey = "value";
constexpr std::string_view itemsKey = "UnlockedItems_V2";
constexpr std::string_view challengesKey = "ChallengesDone_V2";
constexpr std::array<std::string_view, 8> oldDefaultItems{
    "HookRope",
    "AllStatsUp",
    "ShieldedShots",
    "LavaBoots",
    "Drone_101",
    "ProjectileShield",
    "DamageUpLarge",
    "FasterShots",
};
constexpr std::array<ReplacementInfo, 3> itemReplacements{{
    {"DarkMissiles", "ViralInfection"},
    {"FireRateUpOnCrit", "CritRing"},
    {"SuperJumpBoots", "BoostedJumps"},
}};
constexpr std::array<ReplacementInfo, 9> challengeReplacements{{
    {"Boxing Champ", "BoxingChamp"},
    {"Mountain of Corpses", "MountainOfCorpses"},
    {"No Fear!", "NoFear"},
    {"Out of stock", "OutOfStock"},
    {"Secret Room", "SecretRoom"},
    {"Something to cool your anger...", "IceCube"},
    {"Teamwork Needed!", "TeamworkNeeded"},
    {"The Stomper!", "TheStomper"},
    {"Bomb Specialist", "BombSpecialist"},
}};
constexpr std::array<ReplacementInfo, 3> detectionPairs{{
    {"Continue_Coins", "Continue_EventsTyps"},
    {"Continue2", "Continue3"},
    {"Continue_YourFavoriteEnemy2", "DieXTimes"},
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
	auto& array = json[key][valueKey];
	for(const auto& [oldThing, newThing] : collection)
	{
		auto oldVal = find(array, oldThing);
		if(oldVal != array.end())
		{
			*oldVal = newThing;
		}
	}
}

template<typename TCollection>
void append(nlohmann::json& json, std::string_view key, const TCollection& collection)
{
	auto& array = json[key][valueKey];
	for(const auto& item : collection)
	{
		array += item;
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
			ultraItem.value()[valueKey] = legacyItem.value()[valueKey];
		}
	}

	replace(ultraJson, itemsKey, itemReplacements);
	replace(ultraJson, challengesKey, challengeReplacements);

	if(options & UpgradeOptionFlags::OldDefaults)
	{
		append(ultraJson, itemsKey, oldDefaultItems);
	}

	return {ultraJson.dump()};
}

template<typename TCollection>
Version tryDetectVersion(const std::string& content, const TCollection& collection)
{
	for(const auto& [oldThing, newThing] : collection)
	{
		if(content.find(oldThing) != std::string::npos)
		{
			return Legacy;
		}
		if(content.find(newThing) != std::string::npos)
		{
			return Ultra;
		}
	}
	return Unknown;
}

Version tryDetectVersion(const std::string& fileName, const std::string& content)
{
	for(const auto& item : oldDefaultItems)
	{
		if(content.find(item) != std::string::npos)
		{
			return Version::Ultra;
		}
	}
	if(const Version version = tryDetectVersion(content, detectionPairs); version != Unknown)
	{
		return version;
	}
	if(const Version version = tryDetectVersion(content, itemReplacements); version != Unknown)
	{
		return version;
	}
	if(const Version version = tryDetectVersion(content, challengeReplacements); version != Unknown)
	{
		return version;
	}
	const rgt::Version version = fileName.find("SaveData__Deluxe") == 0 ? rgt::Ultra
	                             : fileName.find("SaveData") == 0       ? rgt::Legacy
	                                                                    : rgt::Unknown;
	return version;
}
} // namespace rgt
