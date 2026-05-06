#include "Rendering/SceneRenders/SceneRenderer.h"
#include "Rendering/SceneRenders/ForwardSceneRenderer.h"

std::unique_ptr<SceneRenderer> SceneRenderer::Create(const struct RenderSettings& InSettings)
{
	switch (InSettings.RenderingPath)
	{
	case ERenderingPath::ForwardRendering:
		return std::make_unique<ForwardSceneRenderer>();
	}

	return nullptr;
}
