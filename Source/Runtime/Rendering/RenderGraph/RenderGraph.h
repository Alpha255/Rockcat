#pragma once

#include "Rendering/RenderGraph/RenderPass.h"
#include "Rendering/RenderGraph/RenderPassParameters.h"

class RDGRenderGraph
{
public:
	RDGRenderGraph(const struct RenderSettings& Settings);

	template<class LAMBDA>
	std::shared_ptr<RDGRenderPass> AddPass(RDGEvent&& Event, ERDGPassFlags Flags, LAMBDA&& Lambda)
	{
		using RDGPassType = RDGLambdaRenderPass<LAMBDA>;

		return m_Passes.emplace_back(std::make_shared<RDGPassType>(std::forward<RDGEvent>(Event), Flags, std::forward<LAMBDA>(Lambda)));
	}

	void Execute();

	RDGTexture* CreateTexture(const RHITextureDesc& Desc);
	RDGBuffer* CreateBuffer(const RHIBufferDesc& Desc);

	const struct RenderSettings& GetRenderSettings() const { return m_Settings; }
private:
	void Compile();

	std::vector<std::shared_ptr<RDGRenderPass>> m_Passes;
	const struct RenderSettings& m_Settings;
};