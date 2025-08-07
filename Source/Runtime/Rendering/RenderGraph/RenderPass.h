#pragma once

#include "Rendering/RenderGraph/RenderScene.h"

class RenderPass
{
public:
	RenderPass(DAGNodeID ID, const char* Name, class RenderGraph& Graph);

	const char* GetName() const { return m_Name.data(); }
	void SetName(const char* Name) { m_Name = Name; }

	DAGNodeID GetNodeID() const { return m_NodeID; }

	RDGResource& AddInput(RDGResource::EType Type, const char* Name) { return AddResource(Type, Name, RDGResource::EVisibility::Input); }
	RDGResource& AddOutput(RDGResource::EType Type, const char* Name) { return AddResource(Type, Name, RDGResource::EVisibility::Output); }
	RDGResource& AddInputOutput(RDGResource::EType Type, const char* Name) { return AddResource(Type, Name, RDGResource::EVisibility::Input | RDGResource::EVisibility::Output); }
	RDGResource& AddInternal(RDGResource::EType Type, const char* Name) { return AddResource(Type, Name, RDGResource::EVisibility::Internal); }

	virtual void Compile() = 0;
	virtual void Execute(const RenderScene&) = 0;
	virtual void OnGUI() {};
protected:
	class ResourceManager& GetResourceManager();

	inline class RenderGraph& GetGraph() { return m_Graph; }
	const RenderSettings& GetRenderSettings() const;

	RDGResource& AddResource(RDGResource::EType Type, const char* Name, RDGResource::EVisibility Visibility);

	void AddField(RDGResourceID ID, RDGResource::EVisibility Visibility);

	void AddResource(RDGResource& Resource, RDGResource::EVisibility Visibility)
	{
		Resource.SetVisibility(Visibility);
		AddField(Resource.GetID(), Visibility);
	}

	std::vector<const RDGResource*> GetOutputs() const { return GetResources(RDGResource::EVisibility::Output); }
	std::vector<const RDGResource*> GetInputs() const { return GetResources(RDGResource::EVisibility::Input); }
	std::vector<const RDGResource*> GetInputOutputs() const { return GetResources(RDGResource::EVisibility::Input | RDGResource::EVisibility::Output); }
	std::vector<const RDGResource*> GetInternals() const { return GetResources(RDGResource::EVisibility::Internal); }
private:
	std::vector<const RDGResource*> GetResources(RDGResource::EVisibility Visibility) const;

	DAGNodeID m_NodeID;
	std::string_view m_Name;
	std::vector<RenderPassField> m_Fields;
	class RenderGraph& m_Graph;
};

struct RDGResourceDesc
{
	enum class EVisibility
	{
		None = 0x0,
		Input = 0x1,
		Output = 0x2,
		Internal = 0x4,
		External = 0x8
	};

	enum class EType
	{
		Buffer,
		Texture,
	};

	EVisibility Visibility = EVisibility::None;
	EType Type = EType::Buffer;

	std::string_view Name;
};

class RenderPass2
{
public:
	RenderPass2(const char* Name)
		: m_Name(Name)
	{
	}

	const char* GetName() const { return m_Name.data(); }
	void SetName(const char* Name) { m_Name = Name; }

	virtual void Setup(class ResourceManager& ResourceMgr) = 0;
	virtual void Compile() {}
	virtual void Execute(class RenderContext& Context) = 0;
private:
	std::string_view m_Name;
};
