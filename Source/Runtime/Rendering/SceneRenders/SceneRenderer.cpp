#include "Rendering/SceneRenders/SceneRenderer.h"
#include "Rendering/SceneRenders/ForwardSceneRenderer.h"

SceneRenderer::SceneRenderer(const Scene& InScene, const std::vector<SceneView>& InViews)
	: m_Scene(InScene)
	, m_Views(InViews)
{
}

std::unique_ptr<SceneRenderer> SceneRenderer::Create(ERenderingPath RenderingPath, const class Scene& InScene, const std::vector<SceneView>& InViews)
{
	switch (RenderingPath)
	{
	case ERenderingPath::ForwardRendering:
		return std::make_unique<ForwardSceneRenderer>(InScene, InViews);
	}

	return nullptr;
}
