#include "Applications/RenderTest/RenderTest.h"
#include "Engine/Scene/Scene.h"
#include "Engine/RHI/RHIBackend.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Services/TaskFlowService.h"
#include "Engine/Services/ShaderLibrary.h"
#include "Engine/Asset/GlobalShaders.h"

void RenderTest::Initialize()
{
	m_Scene = Scene::Load<Scene>("RenderTest.scene");
	m_RenderGraph = RenderGraph::Create(GetRenderBackend().GetDevice(), GetRenderSettings(), GetRenderViewport());
}

void RenderTest::Tick(float ElapsedSeconds)
{
	m_Scene->Tick(ElapsedSeconds);
}

void RenderTest::RenderFrame()
{
	m_RenderGraph->Execute(*m_Scene);
}

RUN_APPLICATION(RenderTest, "RenderTest.json")
