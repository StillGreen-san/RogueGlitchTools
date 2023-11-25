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
	emscripten_log(EM_LOG_INFO, "Size: %d, Content: %.60s", fileContent.size(), fileContent.data());
	return {1, 0, 1};
}

EMSCRIPTEN_BINDINGS(std)
{
	emscripten::register_vector<unsigned char>("VectorBytes");
}

EMSCRIPTEN_BINDINGS(RGSU)
{
	emscripten::function("tryUpgrade", &tryUpgrade);
}
