#pragma once

#include "Runtime/Core/Math/Transform.h"
#include "Runtime/Engine/Rendering/RenderGraph/RenderPass.h"

class MeshPass : public RenderPass
{
public:
	enum class EMeshDrawFilter
	{
		All,
		Opaque = 1 << 0,
		Translucent = 1 << 1,
		ShadowCaster = 1 << 2
	};

	struct MeshDrawCommand
	{
		const class StaticMesh* Mesh = nullptr;
		const Math::Transform* WorldTransform = nullptr;
		uint32_t NumInstances = 1u;

		bool8_t CastShadow = true;
		bool8_t IsTranslucent = false;
		bool8_t IsSelected = false;
		EMeshDrawFilter Filter = EMeshDrawFilter::Opaque;
	};

	MeshPass(DAGNodeID ID, const char8_t* Name, class ResourceManager& ResourceMgr, EMeshDrawFilter Filter)
		: RenderPass(ID, Name, ResourceMgr)
		, m_Filter(Filter)
	{
	}

	void Execute(class RHIDevice& RenderDevice, const class Scene& RenderScene);
protected:
private:
	bool8_t NeedRecomputeSceneVisibility(const class Scene& RenderScene) const;

	void GenerateMeshDrawCommands(const class Scene& RenderScene);
	void GenerateVisibleMeshDrawCommands(const std::vector<MeshDrawCommand>& MeshDrawCommands);

	EMeshDrawFilter m_Filter;
	static std::vector<MeshDrawCommand> s_Commands;
};

ENUM_FLAG_OPERATORS(MeshPass::EMeshDrawFilter)