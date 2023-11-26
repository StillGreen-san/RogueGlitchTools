#include "RogueGlitchDencrypter.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

#include <string>
#include <vector>

std::vector<unsigned char> tryUpgrade(const std::string& fileContent)
{
	return upgrade(reinterpret_cast<const unsigned char*>(fileContent.data()), fileContent.size());
}

EMSCRIPTEN_BINDINGS(std)
{
	emscripten::register_vector<unsigned char>("VectorBytes");
}

EMSCRIPTEN_BINDINGS(RGSU)
{
	emscripten::function("tryUpgrade", &tryUpgrade);
}
