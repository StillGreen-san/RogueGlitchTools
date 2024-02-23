#include "convert.hpp"
#include "crypto.hpp"
#include "data.hpp"
#include "file.hpp"
#include "types.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>
#include <variant>

using namespace rgt;

template<Version TVersion>
struct Tag;
template<>
struct Tag<Ultra>
{
	static constexpr Version version = Ultra;
	static constexpr std::string_view filePath = ULTRA_FILE_PATH;
};
template<>
struct Tag<Legacy>
{
	static constexpr Version version = Legacy;
	static constexpr std::string_view filePath = LEGACY_USED_FILE_PATH;
};

TEMPLATE_TEST_CASE("convert integration", "", Tag<Legacy>, Tag<Ultra>)
{
	constexpr std::string_view expectedKey = "UnlockedItems_V2";
	constexpr std::string_view expectedItem = "ProjectileShield";
	constexpr std::string_view unexpectedItem = "FireRateUpOnCrit";
	constexpr std::string_view unexpectedChallenge = "Something to cool your anger...";
	constexpr Version TVersion = TestType::version;
	constexpr std::string_view TFilePath = TestType::filePath;

	const std::string file = loadFile(TFilePath);

	const std::variant decryptedResult = tryDecrypt(file);

	REQUIRE(std::holds_alternative<DecryptedSave<TVersion>>(decryptedResult));

	const auto& decryptedSave = std::get<DecryptedSave<TVersion>>(decryptedResult);

	const DecryptedSave<Ultra> upgradedSave =
	    rgt::upgrade({static_cast<std::string>(decryptedSave)}, UpgradeOptionFlags::None);

	REQUIRE(upgradedSave.find(expectedKey) != std::string::npos);
	REQUIRE(upgradedSave.find(unexpectedItem) == std::string::npos);
	REQUIRE(upgradedSave.find(unexpectedChallenge) == std::string::npos);

	const DecryptedSave<Ultra> upgradedSaveWithOldDefaults =
	    rgt::upgrade({static_cast<std::string>(decryptedSave)}, UpgradeOptionFlags::OldDefaults);

	REQUIRE(upgradedSaveWithOldDefaults.find(expectedKey) != std::string::npos);
	REQUIRE(upgradedSaveWithOldDefaults.find(expectedItem) != std::string::npos);
	REQUIRE(upgradedSaveWithOldDefaults.find(unexpectedItem) == std::string::npos);
	REQUIRE(upgradedSaveWithOldDefaults.find(unexpectedChallenge) == std::string::npos);
}
