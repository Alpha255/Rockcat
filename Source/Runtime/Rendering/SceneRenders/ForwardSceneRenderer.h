#pragma once

#include "Rendering/SceneRenders/SceneRenderer.h"

class ForwardSceneRenderer : public SceneRenderer
{
public:
	using SceneRenderer::SceneRenderer;

	void Render(class RDGRenderGraph& Graph, const struct RDGSceneViewInfo& ViewInfo) override final;
private:
	enum class EForwardRenderStrategy
	{
		Forward,
		ForwardPlus,
	};
};
