#include "saveupgrade.hpp"

#include "RogueGlitchDencrypter.hpp"
#include "app.hpp"

#include <raygui.h>
#include <raylib-cpp.hpp>

#include <string_view>

#if defined(PLATFORM_WEB)
#include "emscripten_browser_file.h"
// #include <emscripten/emscripten.h>
#endif

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

struct ReplacementInfo
{
	std::string_view oldThing;
	std::string_view newThing;
};

void SaveUpgrade::UpdateDraw([[maybe_unused]] App& app)
{
	::BeginDrawing();

	::ClearBackground(::GetColor(static_cast<unsigned>(::GuiGetStyle(DEFAULT, BACKGROUND_COLOR))));

	if(::IsFileDropped())
	{
		::FilePathList filePaths = ::LoadDroppedFiles();
		unsigned fileSize{};
		unsigned char* fileData = ::LoadFileData(filePaths.paths[0], &fileSize);

		//! cannot be array due to some linker bug
		const std::vector<ReplacementInfo> replacements{{
		    {"System.Int32,mscorlib", "int"},      //
		    {"System.Boolean,mscorlib", "bool"},   //
		    {"DarkMissiles", "ViralInfection"},    //
		    {"FireRateUpOnCrit,", "CritRing"},     //
		    {"SuperJumpBoots,", "BoostedJumps"},   //
		    {"Version=2.0.0.0", "Version=4.0.0.0"} //
		}};
		std::string saveData = ::decrypt(fileData, fileSize);

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
				saveData.replace(pos, info.oldThing.size(), info.newThing);
			}
		}

		std::vector<unsigned char> newData = ::encrypt(saveData);

#if defined(PLATFORM_WEB)
		emscripten_browser_file::download(
		    "SaveData__Deluxe.glitch", "application/octet-stream", newData.data(), newData.size());
#endif

		::UnloadFileData(fileData);
		::UnloadDroppedFiles(filePaths);
	}

	::DrawText("Upgrade Lagacy SaveFile", 106, 120, FONT_SIZE, DARKGRAY);
	if(::GuiButton({100, 160, 400, 45}, "Drop SaveFile Here"))
	{
	}

	::EndDrawing();
}
} // namespace raymino
