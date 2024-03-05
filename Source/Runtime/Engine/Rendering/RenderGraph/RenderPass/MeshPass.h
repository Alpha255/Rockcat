#pragma once

#include "Core/Math/Transform.h"
#include "Engine/Rendering/RenderGraph/RenderPass.h"

class MeshPass : public RenderPass
{
public:
	MeshPass(DAGNodeID ID, const char* Name, class ResourceManager& ResourceMgr, EMeshPass Filter)
		: RenderPass(ID, Name, ResourceMgr)
		, m_Filter(Filter)
	{
	}

	void Execute(class RHIDevice& Device, const RenderScene& Scene);

	const RHIGraphicsPipeline* GetGraphicsPipeline() const { assert(m_GraphicsPipeline); return m_GraphicsPipeline.get(); }
protected:
	RHIGraphicsPipelinePtr m_GraphicsPipeline;
private:
	EMeshPass m_Filter;
};