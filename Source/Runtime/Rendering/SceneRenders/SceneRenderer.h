#pragma once

#include "Rendering/RenderSettings.h"

class SceneRenderer
{
public:
	SceneRenderer(const class Scene& InScene, const std::vector<class SceneView>& InViews);

	virtual void Render(class RDGRenderGraph& RGD) = 0;

	static std::unique_ptr<SceneRenderer> Create(ERenderingPath RenderingPath, const class Scene& InScene, const std::vector<class SceneView>& InViews);
private:
	const class Scene& m_Scene;
	const std::vector<class SceneView>& m_Views;
};