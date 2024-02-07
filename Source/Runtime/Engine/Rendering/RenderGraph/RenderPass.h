#pragma once

#include "Engine/Rendering/RenderGraph/RenderPassField.h"
#include "Core/Math/Transform.h"

enum EMeshPass
{
	PreDepth,
	Opaque,
	Translucent,
	ShadowCaster,
	Num
};

struct MeshDrawCommand
{
	const class StaticMesh* Mesh = nullptr;
	const Math::Transform* WorldTransform = nullptr;
	uint32_t NumInstances = 1u;

	bool CastShadow = true;
	bool IsTranslucent = false;
	bool IsSelected = false;
};

class RenderScene
{
public:
	RenderScene(const class Scene& InScene)
		: m_Scene(InScene)
	{
	}

	const std::vector<const MeshDrawCommand*>& GetDrawCommands(EMeshPass MeshPass) const { return m_MeshPassDrawCommands[MeshPass]; }
	void GenerateDrawCommands();
protected:
private:
	const class Scene& m_Scene;
	std::array<std::vector<const MeshDrawCommand*>, EMeshPass::Num> m_MeshPassDrawCommands;
	std::vector<MeshDrawCommand> m_DrawCommands;
};

class RenderPass
{
public:
	RenderPass(DAGNodeID ID, const char* Name, class ResourceManager& ResourceMgr);

	const char* GetName() const { return m_Name.data(); }

	DAGNodeID GetNodeID() const { return m_NodeID; }

	RDGResource& AddInput(const char* Name) { return RegisterResource(Name, RDGResource::EVisibility::Input); }
	RDGResource& AddOutput(const char* Name) { return RegisterResource(Name, RDGResource::EVisibility::Output); }
	RDGResource& AddInputOutput(const char* Name) { return RegisterResource(Name, RDGResource::EVisibility::Input | RDGResource::EVisibility::Output); }
	RDGResource& AddInternal(const char* Name) { return RegisterResource(Name, RDGResource::EVisibility::Internal); }

	virtual void Compile() = 0;

	virtual void Execute(class RHIDevice&, const RenderScene&) = 0;

	virtual void OnGUI() {};
protected:
	class ResourceManager& GetResourceManager() { return m_ResourceMgr; }
	class RHIInterface& GetRHI();
	RDGResource& RegisterResource(const char* Name, RDGResource::EVisibility Visibility);
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
