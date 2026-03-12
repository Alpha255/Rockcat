#pragma once

#include "Scene/SceneView.h"
#include "Rendering/RenderSettings.h"

class SceneRenderer
{
public:
	SceneRenderer(const class Scene&);

	virtual void Render(class RDGRenderGraph& RGD) = 0;

	static std::unique_ptr<SceneRenderer> Create(ERenderingPath RenderingPath, const class Scene& InScene);
private:
	const class Scene& m_Scene;
	std::vector<ISceneView> m_Views;
};