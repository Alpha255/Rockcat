#include "ShadowMapping.h"
#include "Public/Definitions.h"
#include "Public/ImGUI.h"

void ShadowMapping::PrepareScene()
{
}

void ShadowMapping::RenderScene()
{
	REngine::Instance().ResetDefaultRenderSurfaces();
	REngine::Instance().SetViewport(RViewport(0.0f, 0.0f, (float)m_WindowSize.first, (float)m_WindowSize.second));
}
