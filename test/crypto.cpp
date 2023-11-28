#include "crypto.hpp"
#include "data.hpp"
#include "file.hpp"
#include "types.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>

#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

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
	static constexpr std::string_view filePath = LEGACY_FILE_PATH;
};

TEMPLATE_TEST_CASE("crypto integration", "", Tag<Legacy>, Tag<Ultra>)
{
	constexpr std::string_view expectedKey = "UnlockedItems_V2";
	constexpr Version TVersion = TestType::version;
	constexpr std::string_view TFilePath = TestType::filePath;

	const std::variant decryptedFailResult = tryDecrypt(nullptr, 0);

	REQUIRE(std::holds_alternative<std::nullopt_t>(decryptedFailResult));

	const std::vector<unsigned char> file = loadFile(TFilePath);

	const std::variant decryptedResult = tryDecrypt(file.data(), file.size());

	REQUIRE(std::holds_alternative<DecryptedSave<TVersion>>(decryptedResult));

	const auto& decryptedSave = std::get<DecryptedSave<TVersion>>(decryptedResult);

	REQUIRE(decryptedSave.find(expectedKey) != std::string::npos);

	EncryptedSave<TVersion> encryptedSave = encrypt(decryptedSave);

	const std::variant reDecryptedResult = tryDecrypt(encryptedSave.data(), encryptedSave.size());

	REQUIRE(std::holds_alternative<DecryptedSave<TVersion>>(reDecryptedResult));

	const auto& reDecryptedSave = std::get<DecryptedSave<TVersion>>(reDecryptedResult);

	REQUIRE(reDecryptedSave.find(expectedKey) != std::string::npos);
}
