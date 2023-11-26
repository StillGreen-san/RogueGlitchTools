#include "RogueGlitchDencrypter.hpp"

#include <array>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <cryptopp/aes.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/files.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/sha.h>

using CryptoPP::byte;

enum class Version
{
	Unknown,
	Lagacy,
	Ultra,
};

constexpr byte UNUSED = 0;
constexpr size_t BUFSIZE = 16;

std::vector<byte> operator""_byte(const char* bytes, size_t size)
{
	return {(byte*)bytes, (byte*)bytes + size};
}

std::map<Version, std::vector<byte>> esKeys{{Version::Ultra, "YouShouldNotBeAbleToReadThis_1337"_byte},
    {Version::Lagacy, "YouAreACheaterIfYouReadThis_376347"_byte}};

std::string decrypt(const unsigned char* data, unsigned size)
{
	std::array<byte, BUFSIZE> ivBuffer;
	std::copy_n(data, ivBuffer.size(), ivBuffer.data());

	std::array<byte, BUFSIZE> keyBuffer;
	CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1> deriver;

	deriver.DeriveKey(keyBuffer.data(), keyBuffer.size(), UNUSED, esKeys[Version::Lagacy].data(),
	    esKeys[Version::Lagacy].size(), ivBuffer.data(), ivBuffer.size(), 100);

	CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decrypter;

	decrypter.SetKeyWithIV(keyBuffer.data(), keyBuffer.size(), ivBuffer.data(), ivBuffer.size());

	std::string outString;

	CryptoPP::ArraySource fs(data + ivBuffer.size(), size - ivBuffer.size(), true,
	    new CryptoPP::StreamTransformationFilter(decrypter, new CryptoPP::StringSink(outString)));

	return outString;
}

std::vector<unsigned char> encrypt(const std::string& inString)
{
	const std::vector<byte>& password = esKeys[Version::Ultra];

	std::array<byte, BUFSIZE> ivBuffer;
	CryptoPP::AutoSeededRandomPool rng;
	rng.GenerateBlock(ivBuffer.data(), ivBuffer.size());

	std::array<byte, BUFSIZE> keyBuffer;
	CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1> deriver;

	deriver.DeriveKey(keyBuffer.data(), keyBuffer.size(), UNUSED, password.data(), password.size(), ivBuffer.data(),
	    ivBuffer.size(), 100);

	CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encrypter;

	encrypter.SetKeyWithIV(keyBuffer.data(), keyBuffer.size(), ivBuffer.data(), ivBuffer.size());

	std::vector<unsigned char> outVec;

	outVec.insert(outVec.begin(), ivBuffer.begin(), ivBuffer.end());

	CryptoPP::StringSource fs(
	    inString, true, new CryptoPP::StreamTransformationFilter(encrypter, new CryptoPP::VectorSink(outVec)));

	return outVec;
}

struct ReplacementInfo
{
	std::string_view oldThing;
	std::string_view newThing;
};

std::vector<unsigned char> upgrade(const unsigned char* data, unsigned size)
{
	//! cannot be array due to some linker bug
	const std::vector<ReplacementInfo> replacements{{
	    {"System.Int32,mscorlib", "int"},      //
	    {"System.Boolean,mscorlib", "bool"},   //
	    {"DarkMissiles", "ViralInfection"},    //
	    {"FireRateUpOnCrit", "CritRing"},      //
	    {"SuperJumpBoots", "BoostedJumps"},    //
	    {"Version=2.0.0.0", "Version=4.0.0.0"} //
	}};

	std::string saveData = ::decrypt(data, size);

	for(const ReplacementInfo& info : replacements)
	{
		size_t offset = 0;
		while(true)
		{
			size_t pos = saveData.find(info.oldThing, offset);
			if(pos == std::string::npos)
			{
				break;
			}
			offset = pos;
			saveData.replace(pos, info.oldThing.size(), info.newThing);
		}
	}

	return ::encrypt(saveData);
}
