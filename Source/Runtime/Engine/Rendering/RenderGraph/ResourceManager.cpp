#include "Runtime/Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Runtime/Engine/RHI/RHIDevice.h"

ResourceManager::ResourceManager(RHIDevice& RenderDevice, DirectedAcyclicGraph& Graph)
	: m_RenderDevice(RenderDevice)
	, m_Graph(Graph)
{
}

Field& ResourceManager::GetOrAllocateField(
	const char8_t* Name, 
	Field::EVisibility Visibility, 
	Field::EResourceType Type, 
	DAGNodeID RefID)
{
	if (RefID.IsValid())
	{
		return *m_Fields[RefID.GetIndex()];
	}

	auto NewField = std::make_shared<Field>(m_Graph.AddNode(), Name, Visibility, Type);
	m_Fields.insert(std::make_pair(NewField->GetNodeID().GetIndex(), NewField));
	return *NewField;
}

void ResourceManager::CreateAllResources()
{
}
