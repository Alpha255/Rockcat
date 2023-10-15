#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPassField.h"

class RenderPass
{
public:
	RenderPass(DAGNodeID ID, const char8_t* Name, class ResourceManager& ResourceMgr)
		: m_NodeID(ID)
		, m_Name(Name)
		, m_ResourceMgr(ResourceMgr)
	{
		assert(Name);
	}

	const char8_t* GetName() const { return m_Name.data(); }

	class ResourceManager& GetResourceManager() { return m_ResourceMgr; }
	const std::vector<RenderPassField>& GetFields() const { return m_Fields; }
	DAGNodeID GetNodeID() const { return m_NodeID; }

	Field& RegisterField(const char8_t* Name, Field::EVisibility Visibility, Field::EResourceType Type,
		DAGNodeID RefID = DAGNodeID());

	virtual void Execute(class RHIDevice&, const class Scene&) = 0;

	virtual void OnGUI() {};
private:
	DAGNodeID m_NodeID;
	std::string_view m_Name;
	std::vector<RenderPassField> m_Fields;
	class ResourceManager& m_ResourceMgr;
};

#define DECLARE_RENDERPASS_CONSTRUCTOR(Pass, Parent) \
	Pass(DAGNodeID ID, class ResourceManager& ResourceMgr) \
		: Parent(ID, #Pass, ResourceMgr) \
	{} \
