#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass.h"

class RenderGraph
{
public:
	RenderGraph(class RHIInterface& RHI, const class Scene& RenderScene);

	template<class TPass>
	RenderGraph& AddPass()
	{
		m_RenderPasses.emplace_back(std::make_shared<TPass>(m_Graph.AddNode(), *m_ResourceMgr));
		return *this;
	}

	void Execute();

	virtual void Setup() = 0;
private:
	void Compile();

	bool8_t m_NeedRecompile = true;
	DirectedAcyclicGraph m_Graph;
	std::shared_ptr<class ResourceManager> m_ResourceMgr;
	std::shared_ptr<RenderScene> m_RenderScene;
	std::vector<std::shared_ptr<RenderPass>> m_RenderPasses;
};
