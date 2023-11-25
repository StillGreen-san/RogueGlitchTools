#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
#include <string>

extern "C"
{
	EMSCRIPTEN_KEEPALIVE bool tryUpgrade(const unsigned char* fileContent, unsigned size)
	{
		emscripten_log(EM_LOG_INFO, "Size: %d, Content: %.60s", size, reinterpret_cast<const char*>(fileContent));
		return fileContent && size;
	}
}