#include "Rendering/RenderGraph/RenderPass/ShadowPass.h"
#include "Rendering/RenderGraph/RenderGraph.h"
#include "Rendering/RenderSettings.h"

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

//class ShadowPassMeshDrawCommandBuilder : public GeometryPassMeshDrawCommandBuilder<GenericVS, GenericShadow>
//{
//public:
//	using GeometryPassMeshDrawCommandBuilder::GeometryPassMeshDrawCommandBuilder;
//
//	MeshDrawCommand Build(const StaticMesh& Mesh, const Scene&) override final
//	{
//		auto Command = MeshDrawCommand(Mesh);
//		return Command;
//	}
//};

ShadowPass::ShadowPass(DAGNodeID ID, RenderGraph& Graph)
	: GeometryPass(ID, "ShadowPass", Graph)
{
}
