#pragma once

#include "Colorful/IRenderer/RenderGraph/RenderPass.h"

NAMESPACE_START(RHI)

class RenderGraph : public Serializeable<RenderGraph>
{
public:
	RenderGraph(bool8_t EnableAsyncTasks);

	void AddRenderPass(const IRenderPassSharedPtr& RenderPass);

	void RemoveRenderPass(RenderPassID PassID);

	IRenderPassSharedPtr GetRenderPass();

	void Compile();

	void Execute();

	SERIALIZE_START
		CEREAL_NVP(m_Graph)
	SERIALIZE_END
protected:
private:
	bool8_t m_NeedRecompile = true;
	bool8_t m_EnableAsyncTasks = false;
	DirectedAcyclicGraph m_Graph;
	std::vector<IRenderPassSharedPtr> m_RenderPasses;
};

NAMESPACE_END(RHI)
