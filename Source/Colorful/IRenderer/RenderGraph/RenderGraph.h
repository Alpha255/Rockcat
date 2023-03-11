#pragma once

#include "Colorful/IRenderer/RenderGraph/RenderPass.h"

NAMESPACE_START(RHI)

class RenderGraph : public Serializeable<RenderGraph>
{
public:
	RenderGraph(bool8_t EnableAsyncTasks);

	void AddPass(const std::shared_ptr<IRenderPass>& RenderPass);

	void RemovePass(RenderPassID PassID);

	std::shared_ptr<IRenderPass> GetPass();

	void Compile();

	void Execute();

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Graph)
		);
	}
protected:
private:
	bool8_t m_NeedRecompile = true;
	bool8_t m_EnableAsyncTasks = false;
	DirectedAcyclicGraph m_Graph;
	std::vector<std::shared_ptr<IRenderPass>> m_RenderPasses;
};

NAMESPACE_END(RHI)
