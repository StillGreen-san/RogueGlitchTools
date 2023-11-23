#include "saveupgrade.hpp"

#include "app.hpp"

#include <raygui.h>
#include <raylib-cpp.hpp>

// #if defined(PLATFORM_WEB)
// #include <emscripten/emscripten.h>
// #endif

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

void SaveUpgrade::UpdateDraw([[maybe_unused]] App& app)
{
	::BeginDrawing();

	::ClearBackground(::GetColor(static_cast<unsigned>(::GuiGetStyle(DEFAULT, BACKGROUND_COLOR))));

	if(::GuiButton({190, 278, 200, 30}, "Upgrade Lagacy SaveFile"))
	{
	}

	::EndDrawing();
}
} // namespace raymino
