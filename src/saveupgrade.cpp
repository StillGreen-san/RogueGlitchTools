#include "saveupgrade.hpp"

#include "RogueGlitchDencrypter.hpp"
#include "app.hpp"

#include <Functions.hpp>
#include <raygui.h>
#include <raylib-cpp.hpp>

#include <string>
#include <string_view>
#include <vector>

#if defined(PLATFORM_WEB)
#include "emscripten_browser_file.h"
// #include <emscripten/emscripten.h>
#endif

namespace raylib
{
std::vector<unsigned char> LoadFileData(const std::string& fileName)
{
	unsigned bytes = 0;
	unsigned char* data = ::LoadFileData(fileName.c_str(), &bytes);
	std::vector<unsigned char> output(data, data + bytes);
	::UnloadFileData(data);
	return output;
}
} // namespace raylib

namespace
{
constexpr int FONT_SIZE = 30;
}

namespace raymino
{
template<>
std::unique_ptr<IScene> MakeScene<Scene::SaveUpgrade>(App& app)
{
	return std::make_unique<SaveUpgrade>(app);
}

SaveUpgrade::SaveUpgrade(App& app)
{
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, FONT_SIZE);
	::GuiLock();
}

void SaveUpgrade::PreDestruct([[maybe_unused]] App& app)
{
}

void SaveUpgrade::UpdateDraw([[maybe_unused]] App& app)
{
	::BeginDrawing();

	::ClearBackground(::GetColor(static_cast<unsigned>(::GuiGetStyle(DEFAULT, BACKGROUND_COLOR))));

	if(::IsFileDropped())
	{
		const std::vector<std::string> filePaths = raylib::LoadDroppedFiles();
		const std::vector<unsigned char> fileData = raylib::LoadFileData(filePaths.front());

		const std::vector<unsigned char> newData = upgrade(fileData.data(), fileData.size());

		::TraceLog(LOG_INFO, "RGSU: file upgraded");
		::TraceLog(LOG_INFO, "RGSU: send file to user");

#if defined(PLATFORM_WEB)
		emscripten_browser_file::download(
		    "SaveData__Deluxe.glitch", "application/octet-stream", newData.data(), newData.size());
#endif
	}

	::DrawText("Upgrade Lagacy SaveFile", 106, 120, FONT_SIZE, DARKGRAY);
	if(::GuiButton({100, 160, 400, 45}, "Drop SaveFile Here"))
	{
	}

	::EndDrawing();
}
} // namespace raymino
