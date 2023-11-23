#pragma once

#include "app.hpp"
#include "scenes.hpp"

namespace raymino
{
struct SaveUpgrade : public IScene
{
	explicit SaveUpgrade(App& app);

	void UpdateDraw(App& app) override;
	void PreDestruct(App& app) override;
};
} // namespace raymino
