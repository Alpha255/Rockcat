#pragma once

#include "Rendering/SceneRenders/SceneRenderer.h"

class ForwardSceneRenderer : public SceneRenderer
{
public:
	using SceneRenderer::SceneRenderer;
private:
	enum class EForwardRenderStrategy
	{
		Forward,
		ForwardPlus,
	};
};
