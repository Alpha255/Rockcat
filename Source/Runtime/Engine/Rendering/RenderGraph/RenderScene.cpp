#include "Engine/Rendering/RenderGraph/RenderScene.h"

void RenderScene::GenerateDrawCommands(const Scene& InScene)
{
	m_DrawCommands.clear();
}

const std::vector<std::shared_ptr<IView>>& RenderScene::GetViews() const
{
	return m_Views;
}