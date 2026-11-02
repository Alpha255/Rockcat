#pragma once

#include "Rendering/GraphicsSettings.h"

class SceneRenderer
{
public:
	SceneRenderer() = default;

	virtual void Render(class RDGRenderGraph& Graph, const struct RDGSceneViewInfo& ViewInfo) = 0;

	static std::unique_ptr<SceneRenderer> Create(const struct GraphicsSettings& InSettings);
private:
};