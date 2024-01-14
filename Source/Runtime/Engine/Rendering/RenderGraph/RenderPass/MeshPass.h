#pragma once

#include "Runtime/Core/Math/Transform.h"
#include "Runtime/Engine/Rendering/RenderGraph/RenderPass.h"

class MeshPass : public RenderPass
{
public:
	MeshPass(DAGNodeID ID, const char8_t* Name, class ResourceManager& ResourceMgr, EMeshPass Filter)
		: RenderPass(ID, Name, ResourceMgr)
		, m_Filter(Filter)
	{
	}

	void Execute(class RHIDevice& Device, const RenderScene& Scene);
protected:
private:
	EMeshPass m_Filter;
};