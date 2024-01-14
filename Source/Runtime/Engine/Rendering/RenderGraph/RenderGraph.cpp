#include "Runtime/Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Runtime/Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Runtime/Engine/RHI/RHIInterface.h"
#include "Runtime/Engine/RHI/RHIDevice.h"
#include "Runtime/Engine/Scene/Scene.h"

RenderGraph::RenderGraph(RHIInterface& RHI, const Scene& InScene)
	: m_RHIDevice(RHI.GetDevice())
	, m_RenderScene(std::make_shared<RenderScene>(InScene))
	, m_ResourceMgr(std::move(std::make_shared<ResourceManager>(RHI.GetDevice(), m_Graph)))
{
}

void RenderGraph::Compile()
{
	if (m_NeedRecompile)
	{
		for (auto& Pass : m_RenderPasses)
		{
			Pass->Compile();
		}

		m_RenderScene->GenerateDrawCommands();

		m_NeedRecompile = false;
	}
}

void RenderGraph::Execute()
{
	Compile();

	m_ResourceMgr->CreateResources();

	for (auto& Pass : m_RenderPasses)
	{
		Pass->Execute(m_RHIDevice, *m_RenderScene);
	}
}
