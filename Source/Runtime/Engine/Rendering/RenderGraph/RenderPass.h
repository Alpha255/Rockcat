#pragma once

#include "Engine/Rendering/RenderGraph/RenderScene.h"

class RenderPass
{
public:
	RenderPass(DAGNodeID ID, const char* Name, class RenderGraph& Graph);

	const char* GetName() const { return m_Name.data(); }

	DAGNodeID GetNodeID() const { return m_NodeID; }

	RDGResource& AddInput(RDGResource::EType Type, const char* Name) { return AddResource(Type, Name, RDGResource::EVisibility::Input); }
	RDGResource& AddOutput(RDGResource::EType Type, const char* Name) { return AddResource(Type, Name, RDGResource::EVisibility::Output); }
	RDGResource& AddInputOutput(RDGResource::EType Type, const char* Name) { return AddResource(Type, Name, RDGResource::EVisibility::Input | RDGResource::EVisibility::Output); }
	RDGResource& AddInternal(RDGResource::EType Type, const char* Name) { return AddResource(Type, Name, RDGResource::EVisibility::Internal); }

	virtual void Compile() = 0;

	virtual void Execute(class RHIDevice&, const RenderScene&) = 0;

	virtual void OnGUI() {};
protected:
	class ResourceManager& GetResourceManager();
	const GraphicsSettings& GetGraphicsSettings() const;

	RDGResource& AddResource(RDGResource::EType Type, const char* Name, RDGResource::EVisibility Visibility);
	void AddField(RDGResourceID ID, RDGResource::EVisibility Visibility);

	void AddResource(RDGResource& Resource, RDGResource::EVisibility Visibility)
	{
		Resource.SetVisibility(Visibility);
		AddField(Resource.GetID(), Visibility);
	}
private:
	DAGNodeID m_NodeID;
	std::string_view m_Name;
	std::vector<RenderPassField> m_Fields;
	class RenderGraph& m_Graph;
};

#define DECLARE_RENDERPASS_CONSTRUCTOR(Pass, Parent) \
	Pass(DAGNodeID ID, class RenderGraph& Graph) \
		: Parent(ID, #Pass, Graph) \
	{} \
