#pragma once

#include "Engine/Rendering/RenderGraph/RenderPassField.h"
#include "Core/Math/Transform.h"
#include "Engine/View/View.h"

enum class EGeometryPassFilter
{
	PreDepth,
	Opaque,
	Translucent,
	ShadowCaster,
	Num
};

struct VertexStream
{
	uint16_t Index = 0;
	uint16_t Offset = 0u;
	RHIBuffer* VertexBuffer = nullptr;
};

struct MeshDrawCommand
{
	const class StaticMesh* Mesh = nullptr;
	const Math::Transform* WorldTransform = nullptr;

	std::vector<VertexStream> VertexStreams;
	RHIBuffer* IndexBuffer = nullptr;

	RHIGraphicsPipeline* GraphicsPipeline = nullptr;

	uint32_t FirstIndex = 0u;
	uint32_t NumPrimitives = 0u;
	uint32_t NumInstances = 1u;

	ERHIPrimitiveTopology PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;

	union
	{
		struct
		{
			uint32_t BaseVertexIndex;
			uint32_t NumVertices;
		} VertexArgs;

		struct
		{
			RHIBuffer* Buffer;
			uint32_t Offset;
		} IndirectArgs;
	};

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

	const std::vector<const MeshDrawCommand*>& GetDrawCommands(EGeometryPassFilter MeshPass) const { return m_MeshDrawCommands[(size_t)MeshPass]; }
	void GenerateDrawCommands();
	const std::vector<std::shared_ptr<IView>>& GetViews() const;
protected:
private:
	const class Scene& m_Scene;
	std::array<std::vector<const MeshDrawCommand*>, (size_t)EGeometryPassFilter::Num> m_MeshDrawCommands;
	std::vector<MeshDrawCommand> m_DrawCommands;
	mutable std::vector<std::shared_ptr<IView>> m_Views;
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
