#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPassField.h"

class ResourceManager
{
public:
	ResourceManager(class RHIDevice& RenderDevice, DirectedAcyclicGraph& Graph);

	Field& GetOrAllocateField(const char8_t* Name, Field::EVisibility Visibility, Field::EResourceType Type, 
		DAGNodeID RefID = DAGNodeID());

	void CreateAllResources();
private:
	std::unordered_map<DAGNodeID::IndexType, std::shared_ptr<Field>> m_Fields;
	class RHIDevice& m_RenderDevice;
	DirectedAcyclicGraph& m_Graph;
};