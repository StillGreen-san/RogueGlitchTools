#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
#include <string>

extern "C"
{
	EMSCRIPTEN_KEEPALIVE const char* processFile(const char* fileContent)
	{
		static std::string contentBuffer;
		contentBuffer.assign("Modified: ").append(fileContent);
		return contentBuffer.c_str();
	}
}