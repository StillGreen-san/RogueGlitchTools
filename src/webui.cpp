#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
#include <string>

std::string tryUpgrade(const std::string& fileContent)
{
	emscripten_log(EM_LOG_INFO, "Size: %d, Content: %.60s", fileContent.size(), fileContent.data());
	return fileContent;
}

EMSCRIPTEN_BINDINGS(RGSU)
{
	emscripten::function("tryUpgrade", &tryUpgrade);
}
