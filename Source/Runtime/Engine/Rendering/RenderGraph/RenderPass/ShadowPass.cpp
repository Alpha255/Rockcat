#include "Engine/Rendering/RenderGraph/RenderPass/ShadowPass.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Asset/GlobalShaders/DefaultShading.h"

const char* GetShadowTechniqueName(EShadowTechnique Technique)
{
	switch (Technique)
	{
	case EShadowTechnique::Generic:
		return "GenericShadowMapPass";
	case EShadowTechnique::Cascade:
		return "CascadeShadowMapPass";
	default:
		return "None";
	}
}

class ShadowPassMeshDrawCommandBuilder : public GeometryPassMeshDrawCommandBuilder<GenericVS, GenericShadow>
{
public:
	using GeometryPassMeshDrawCommandBuilder::GeometryPassMeshDrawCommandBuilder;

	MeshDrawCommand Build(const StaticMesh& Mesh, const Scene& InScene) override final
	{
		auto Command = MeshDrawCommand(Mesh);
		return Command;
	}
};

RenderScene::RenderScene(const GraphicsSettings& GfxSettings)
	: m_GfxSettings(GfxSettings)
{
}

ShadowPass::ShadowPass(DAGNodeID ID, RenderGraph& Graph)
	: GeometryPass(
		ID, 
		"ShadowPass",
		Graph,
		EGeometryPassFilter::ShadowCast,
		new ShadowPassMeshDrawCommandBuilder(Graph.GetGraphicsSettings()))
{
}
