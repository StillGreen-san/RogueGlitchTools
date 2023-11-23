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

/*
 public override void Decrypt(Stream input, Stream output, string password, int bufferSize)
   {
     using (Aes aes = Aes.Create())
     {
       byte[] buffer = new byte[16];
       input.Read(buffer, 0, 16);
       aes.IV = buffer;
       Rfc2898DeriveBytes rfc2898DeriveBytes = new Rfc2898DeriveBytes(password, aes.IV, 100);
       aes.Key = rfc2898DeriveBytes.GetBytes(16);
       using (ICryptoTransform decryptor = aes.CreateDecryptor())
       {
         using (CryptoStream input1 = new CryptoStream(input, decryptor, CryptoStreamMode.Read))
           EncryptionAlgorithm.CopyStream((Stream) input1, output, bufferSize);
       }
     }
     output.Position = 0L;
   }
 */

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

int decrypt(std::filesystem::path inFile)
{
	for(const auto& [version, key] : esKeys)
	{
		try
		{
			std::ifstream inStream(inFile, std::ios_base::in | std::ios_base::binary);
			if(!inStream.is_open())
			{
				std::cout << "Could not open input file!" << std::endl;
				return -1;
			}

			std::array<byte, BUFSIZE> ivBuffer;
			inStream.read((char*)ivBuffer.data(), ivBuffer.size());

			std::array<byte, BUFSIZE> keyBuffer;
			CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1> deriver;

			deriver.DeriveKey(keyBuffer.data(), keyBuffer.size(), UNUSED, key.data(), key.size(), ivBuffer.data(),
			    ivBuffer.size(), 100);

			CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decrypter;

			decrypter.SetKeyWithIV(keyBuffer.data(), keyBuffer.size(), ivBuffer.data(), ivBuffer.size());

			std::string outString;

			CryptoPP::FileSource fs(inStream, true,
			    new CryptoPP::StreamTransformationFilter(decrypter, new CryptoPP::StringSink(outString)));

			if(outString.front() != '{')
			{
				continue;
			}

			std::filesystem::path outFile(inFile);
			outFile.replace_extension(".json");

			std::ofstream outStream(outFile, std::ios_base::out | std::ios_base::binary);
			if(!outStream.is_open())
			{
				std::cout << "Could not open output file!" << std::endl;
				return -1;
			}

			outStream << outString;

			return 0;
		}
		catch(...)
		{
		}
	}

	std::cout << "Could decrypt file!" << std::endl;
	return -1;
}

int encrypt(std::filesystem::path file)
{
	std::ifstream inStream(file, std::ios_base::in | std::ios_base::binary);
	if(!inStream.is_open())
	{
		std::cout << "Could not open input file!" << std::endl;
		return -1;
	}

	std::string inString{std::istreambuf_iterator<char>(inStream), std::istreambuf_iterator<char>()};

	inStream.close();

	const std::vector<byte>& password =
	    inString.find("KablooeyChallenge") != std::string::npos ? esKeys[Version::Ultra] : esKeys[Version::Lagacy];

	std::array<byte, BUFSIZE> ivBuffer;
	CryptoPP::AutoSeededRandomPool rng;
	rng.GenerateBlock(ivBuffer.data(), ivBuffer.size());

	std::array<byte, BUFSIZE> keyBuffer;
	CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA1> deriver;

	deriver.DeriveKey(keyBuffer.data(), keyBuffer.size(), UNUSED, password.data(), password.size(), ivBuffer.data(),
	    ivBuffer.size(), 100);

	CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encrypter;

	encrypter.SetKeyWithIV(keyBuffer.data(), keyBuffer.size(), ivBuffer.data(), ivBuffer.size());

	file.replace_extension(".glitch");

	std::ofstream outStream(file, std::ios_base::out | std::ios_base::binary);
	if(!outStream.is_open())
	{
		std::cout << "Could not open output file!" << std::endl;
		return -1;
	}

	outStream.write((char*)ivBuffer.data(), ivBuffer.size());

	CryptoPP::StringSource fs(
	    inString, true, new CryptoPP::StreamTransformationFilter(encrypter, new CryptoPP::FileSink(outStream)));

	return 0;
}
