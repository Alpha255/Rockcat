#pragma once

#include "Rendering/MeshDrawCommand.h"

class RenderScene
{
public:
	RenderScene(const class Scene& InScene);

	const class Scene& GetScene() const { return m_Scene; }
private:

	const class Scene& m_Scene;
};
