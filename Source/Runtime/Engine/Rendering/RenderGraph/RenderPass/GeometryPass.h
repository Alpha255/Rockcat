#pragma once

#include "Core/Math/Transform.h"
#include "Engine/Rendering/RenderGraph/RenderPass.h"

class GeometryPass : public RenderPass
{
public:
	GeometryPass(DAGNodeID ID, const char* Name, class ResourceManager& ResourceMgr, EGeometryPassFilter Filter)
		: RenderPass(ID, Name, ResourceMgr)
		, m_Filter(Filter)
	{
	}

	void Execute(class RHIDevice& Device, const RenderScene& Scene);
protected:
private:
	EGeometryPassFilter m_Filter;
};