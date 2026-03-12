#include "Rendering/SceneRenders/SceneRenderer.h"
#include "Rendering/SceneRenders/ForwardSceneRenderer.h"

SceneRenderer::SceneRenderer(const Scene& InScene)
	: m_Scene(InScene)
{
}

std::unique_ptr<SceneRenderer> SceneRenderer::Create(ERenderingPath RenderingPath, const class Scene& InScene)
{
	switch (RenderingPath)
	{
	case ERenderingPath::ForwardRendering:
		return std::make_unique<ForwardSceneRenderer>(InScene);
	}

	return nullptr;
}
