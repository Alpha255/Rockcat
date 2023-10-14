#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass.h"

class RenderGraph
{
public:
	RenderGraph(class RHIDevice& RenderDevice, class Scene& RenderScene);

	template<class TPass>
	TPass& AddPass()
	{
		auto GraphNodeID = m_Graph.AddNode();
		auto Pass = std::make_shared<TPass>(GraphNodeID, *m_ResourceMgr);
		m_RenderPasses.insert(std::make_pair(GraphNodeID.GetIndex(), Pass));
		return *Pass;
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
	std::unique_ptr<class ResourceManager> m_ResourceMgr;
	std::unordered_map<DAGNodeID::IndexType, std::shared_ptr<RenderPass>> m_RenderPasses;
};
