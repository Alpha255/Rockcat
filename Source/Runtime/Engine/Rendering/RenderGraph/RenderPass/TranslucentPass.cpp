#include "Engine/Rendering/RenderGraph/RenderPass/TranslucentPass.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"

//class TransluentPassMeshDrawCommandBuilder : public GeometryPassMeshDrawCommandBuilder<GenericVS, DefaultLit>
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

TranslucentPass::TranslucentPass(DAGNodeID ID, RenderGraph& Graph)
	: GeometryPass(ID, "TranslucentPass", Graph, EGeometryPassFilter::Translucent, nullptr)
{
}
