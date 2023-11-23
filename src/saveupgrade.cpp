#include "saveupgrade.hpp"

#include "RogueGlitchDencrypter.hpp"
#include "app.hpp"

#include <raygui.h>
#include <raylib-cpp.hpp>

#if defined(PLATFORM_WEB)
#include "emscripten_browser_file.h"
// #include <emscripten/emscripten.h>
#endif

namespace raymino
{
template<>
std::unique_ptr<IScene> MakeScene<Scene::SaveUpgrade>(App& app)
{
	return std::make_unique<SaveUpgrade>(app);
}

SaveUpgrade::SaveUpgrade(App& app)
{
	// #if defined(PLATFORM_WEB)
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
		const std::vector<ReplacementInfo> replacements{{{"System.Int32,mscorlib", "int"},
		    {"System.Boolean,mscorlib", "bool"}, {"Version=2.0.0.0", "Version=4.0.0.0"}}};
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

	if(::GuiButton({190, 278, 200, 30}, "Upgrade Lagacy SaveFile"))
	{
	}

	::EndDrawing();
}
} // namespace raymino
