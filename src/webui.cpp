#include "convert.hpp"
#include "crypto.hpp"
#include "types.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

#include <string>
#include <variant>
#include <vector>

std::vector<unsigned char> tryUpgrade(const std::string& fileContent, unsigned options)
{
	std::variant decryptedResult =
	    rgt::tryDecrypt(reinterpret_cast<const unsigned char*>(fileContent.data()), fileContent.size());
	if(!std::holds_alternative<rgt::DecryptedSave<rgt::Legacy>>(decryptedResult))
	{
		return {};
	}
	const rgt::DecryptedSave<rgt::Ultra> upgradedSave =
	    rgt::upgrade(std::move(std::get<rgt::DecryptedSave<rgt::Legacy>>(decryptedResult)),
	        static_cast<rgt::UpgradeOptionFlags>(options));
	return rgt::encrypt(upgradedSave);
}

EMSCRIPTEN_BINDINGS(std)
{
	emscripten::register_vector<unsigned char>("VectorBytes");
}

EMSCRIPTEN_BINDINGS(rgt)
{
	emscripten::function("tryUpgrade", &tryUpgrade);

	emscripten::enum_<rgt::UpgradeOptionFlags>("UpgradeOptions")     //
	    .value("None", rgt::UpgradeOptionFlags::None)                //
	    .value("OldDefaults", rgt::UpgradeOptionFlags::OldDefaults); //
}
