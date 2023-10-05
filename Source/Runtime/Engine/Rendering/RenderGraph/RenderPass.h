#pragma once

#include "Runtime/Core/DirectedAcyclicGraph.h"
#include "Runtime/Engine/Rendering/RenderGraph/RenderPassField.h"

class RenderPass : public DirectedAcyclicGraph::Node
{
public:
	RenderPass(DirectedAcyclicGraph::NodeID ID, const char8_t* Name, class ResourceManager& ResourceMgr)
		: DirectedAcyclicGraph::Node(ID)
		, m_Name(Name)
		, m_ResourceMgr(ResourceMgr)
	{
	}

	const char8_t* GetName() const { return m_Name.data(); }
	class ResourceManager& GetResourceManager() { return m_ResourceMgr; }

	Field& RegisterField(const char8_t* Name, Field::EVisibility Visibility, Field::EResourceType Type);

	virtual void Execute(class RHIDevice&) = 0;
private:
	std::string_view m_Name;
	std::vector<RenderPassField> m_Fields;
	class ResourceManager& m_ResourceMgr;
};
