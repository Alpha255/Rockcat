#include "Applications/RenderTest/RenderTest.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/RHI/RHIBackend.h"
#include "Engine/Services/RenderService.h"

void RenderTest::InitializeImpl()
{
	m_Scene = Scene::Load<Scene>("RenderTest.scene");
	m_RenderGraph = RenderGraph::Create(GetRenderSettings(), GetRenderViewport());
}

void RenderTest::RenderFrame(RHITexture* BackBuffer)
{
	m_RenderGraph->Execute(*m_Scene);
}

RUN_APPLICATION(RenderTest, "RenderTest.json")
