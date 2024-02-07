#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"

class PreDepthPass : public MeshPass
{
public:
	PreDepthPass(DAGNodeID ID, class ResourceManager& ResourceMgr)
		: MeshPass(ID, "PreDepthPass", ResourceMgr, EMeshPass::PreDepth)
	{
	}

	void Compile() override final;

	RHIGraphicsPipeline* GetOverrideGraphicsPipeline() override final { return m_Pipeline.get(); }
private:
	RHIGraphicsPipelinePtr m_Pipeline;
};
