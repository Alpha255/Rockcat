#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass.h"

class RenderGraph
{
public:
	RenderGraph(class RHIInterface& RHI, class Scene& RenderScene);

	template<class TPass>
	RenderGraph& AddPass()
	{
		m_RenderPasses.emplace_back(std::make_shared<TPass>(m_Graph.AddNode(), *m_ResourceMgr));
		return *this;
	}

	void Execute();

	const Scene& GetScene() const { return m_RenderScene; }

	virtual void Setup() = 0;
protected:
private:
	void Compile();

	bool8_t m_NeedRecompile = true;
	DirectedAcyclicGraph m_Graph;
	class RHIDevice& m_RenderDevice;
	const class Scene& m_RenderScene;
	std::shared_ptr<class ResourceManager> m_ResourceMgr;
	std::vector<std::shared_ptr<RenderPass>> m_RenderPasses;
};
