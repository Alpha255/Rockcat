#pragma once

#include "Scene/SceneView.h"

class SceneRenderer
{
public:
	SceneRenderer(const class Scene&);

	virtual void Render(class RDGRenderGraph& RGD) = 0;
private:
	const class Scene& m_Scene;
	std::vector<ISceneView> m_Views;
};