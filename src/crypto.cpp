#include "crypto.hpp"
#include "types.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include <cryptopp/aes.h>
#include <cryptopp/config_int.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/sha.h>

using CryptoPP::byte;

namespace
{
constexpr byte UNUSED = 0;
constexpr size_t CRYPT_BUFSIZE = 16;
constexpr size_t CRYPT_ITERATIONS = 100;

struct ByteView
{
	const byte* data;
	size_t size;
};

ByteView operator""_byte(const char* bytes, size_t size)
{
	return {reinterpret_cast<const byte*>(bytes), size}; // NOLINT(*-pro-type-reinterpret-cast)
}

struct KeyInfo
{
	rgt::Version version;
	ByteView key;
};

const std::array<KeyInfo, 2> ES_KEYS = [] // NOLINT(*-statically-constructed-objects, *-err58-cpp)
{                                         // cannot be constexpr due to reinterpret cast, does not throw exception
	std::array<KeyInfo, 2> keys{};
	keys[rgt::Legacy] = {rgt::Legacy, "YouAreACheaterIfYouReadThis_376347"_byte};
	keys[rgt::Ultra] = {rgt::Ultra, "YouShouldNotBeAbleToReadThis_1337"_byte};
	return keys;
}();
} // namespace

namespace rgt
{
std::variant<DecryptedSave<Legacy>, DecryptedSave<Ultra>, std::nullopt_t> tryDecrypt(const std::string& content)
{
	if(content.size() <= CRYPT_BUFSIZE)
	{
		return std::nullopt;
	}
	const byte* data = reinterpret_cast<const byte*>(content.data());
	const size_t size = content.size();

	for(const auto [version, key] : ES_KEYS)
	{
		try
		{
			std::array<byte, CRYPT_BUFSIZE> ivBuffer; // NOLINT(*-member-init) initialized below
			std::copy_n(data, ivBuffer.size(), ivBuffer.data());

			std::array<byte, CRYPT_BUFSIZE> keyBuffer; // NOLINT(*-member-init) initialized below
			const CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1> deriver;

			deriver.DeriveKey(keyBuffer.data(), keyBuffer.size(), UNUSED, key.data, key.size, ivBuffer.data(),
			    ivBuffer.size(), CRYPT_ITERATIONS);

			CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decrypter;

			decrypter.SetKeyWithIV(keyBuffer.data(), keyBuffer.size(), ivBuffer.data(), ivBuffer.size());

			std::string outString;

			const CryptoPP::ArraySource decryptPipeline(std::next(data, ivBuffer.size()), size - ivBuffer.size(), true,
			    new CryptoPP::StreamTransformationFilter(decrypter, new CryptoPP::StringSink(outString)));

			if(version == Ultra)
			{
				return DecryptedSave<Ultra>{std::move(outString)};
			}
			return DecryptedSave<Legacy>{std::move(outString)};
		}
		catch(...) // NOLINT(*-empty-catch) Error handled by way of trying all keys and returning nullopt if all fail
		{
		}
	}

	return std::nullopt;
}

std::string encrypt(const std::string& save, Version version)
{
	std::array<byte, CRYPT_BUFSIZE> ivBuffer; // NOLINT(*-member-init) initialized below
	CryptoPP::AutoSeededRandomPool rng;
	rng.GenerateBlock(ivBuffer.data(), ivBuffer.size());

	std::array<byte, CRYPT_BUFSIZE> keyBuffer; // NOLINT(*-member-init) initialized below
	const CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1> deriver;

	const auto [keyData, keySize] = ES_KEYS[version].key;
	deriver.DeriveKey(keyBuffer.data(), keyBuffer.size(), UNUSED, keyData, keySize, ivBuffer.data(), ivBuffer.size(),
	    CRYPT_ITERATIONS);

	CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encrypter;

	encrypter.SetKeyWithIV(keyBuffer.data(), keyBuffer.size(), ivBuffer.data(), ivBuffer.size());

	std::string outString;

	outString.insert(outString.begin(), ivBuffer.begin(), ivBuffer.end());

	const CryptoPP::StringSource encryptPipeline(
	    save, true, new CryptoPP::StreamTransformationFilter(encrypter, new CryptoPP::StringSink(outString)));

	return outString;
}

template<>
EncryptedSave<Legacy> encrypt<Legacy>(const DecryptedSave<Legacy>& save)
{
	return {encrypt(save, Legacy)};
}
template<>
EncryptedSave<Ultra> encrypt<Ultra>(const DecryptedSave<Ultra>& save)
{
	return {encrypt(save, Ultra)};
}
} // namespace rgt
