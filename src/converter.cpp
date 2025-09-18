#include "convert.hpp"
#include "crypto.hpp"
#include "dependency_info.hpp"
#include "file.hpp"
#include "types.hpp"

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		std::cout << "Incorrect Number of arguments!\n";
		std::cout << "Specify a path to 1 encrypted '.glitch' or decrypted '.json' file.\n";
		std::cout << "Or '--about' to show about information\n";
		return -1;
	}

	const std::string_view argv1{argv[1]}; // NOLINT(*-pro-bounds-pointer-arithmetic)

	if(argv1 == "--about")
	{
		std::cout << rgt::getAboutString();
		return 0;
	}

	std::filesystem::path file(argv1);

	if(!std::filesystem::exists(file))
	{
		std::cout << "File does not exist!\n";
		return -1;
	}

	if(file.extension() == ".glitch")
	{
		const std::string fileContent = rgt::loadFile(file);
		const std::variant decryptedSave = rgt::tryDecrypt(fileContent);
		const std::optional decryptedContent = std::visit(
		    [](auto&& arg) -> std::optional<std::string>
		    {
			    if constexpr(std::is_same_v<std::decay<decltype(arg)>, std::nullopt_t>)
			    {
				    return std::nullopt;
			    }
			    else
			    {
				    return arg;
			    }
		    },
		    decryptedSave);
		if(!decryptedContent)
		{
			std::cout << "File could not be decrypted!\n";
			return -1;
		}
		if(!rgt::saveFile(file.replace_extension(".json"), *decryptedContent))
		{
			std::cout << "File could not be saved!\n";
			return -1;
		}
		return 0;
	}
	if(file.extension() == ".json")
	{
		const std::string fileContent = rgt::loadFile(file);
		const rgt::Version version = rgt::tryDetectVersion(file.filename().string(), fileContent);
		switch(version)
		{
		case rgt::Legacy:
		{
			const rgt::EncryptedSave<rgt::Legacy> encryptedSave =
			    rgt::encrypt(rgt::DecryptedSave<rgt::Legacy>{fileContent});
			if(!rgt::saveFile(file.replace_extension(".glitch"), encryptedSave))
			{
				std::cout << "File could not be saved!\n";
				return -1;
			}
			return 0;
		}
		case rgt::Ultra:
		{
			const rgt::EncryptedSave<rgt::Ultra> encryptedSave =
			    rgt::encrypt(rgt::DecryptedSave<rgt::Ultra>{fileContent});
			if(!rgt::saveFile(file.replace_extension(".glitch"), encryptedSave))
			{
				std::cout << "File could not be saved!\n";
				return -1;
			}
			return 0;
		}
		case rgt::Unknown:
		default:
			std::cout << "File version could not be determined!\n";
			return -1;
		}
	}

	std::cout << "Unknown file extension!\n";
	return -1;
}
