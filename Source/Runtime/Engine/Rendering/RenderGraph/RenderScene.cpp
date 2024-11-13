#include "Engine/Rendering/RenderGraph/RenderScene.h"
#include "Engine/Scene/Scene.h"

MeshDrawCommand::MeshDrawCommand(const StaticMesh& Mesh, const MaterialAsset* InMaterial, GeometryPassGraphicsPipelineBuilder* PipelineBuilder)
	: IndexBuffer(Mesh.GetIndexBuffer())
	, Material(InMaterial)
	, FirstIndex(0u)
	, NumPrimitives(Mesh.GetNumPrimitive())
	, NumIndex(Mesh.GetNumIndex())
	, PrimitiveTopology(Mesh.GetPrimitiveTopology())
	, IndexFormat(Mesh.GetIndexFormat())
{
	assert(PipelineBuilder);

	VertexArgs.BaseIndex = 0u;
	VertexArgs.NumVertex = Mesh.GetNumVertex();

	PipelineBuilder->Build(*this);
}

class PreDepthPassPipelineBuilder : public GeometryPassGraphicsPipelineBuilder
{
public:
	void Build(MeshDrawCommand& Command) override
	{
	}
};

RenderScene::RenderScene()
{
}

void RenderScene::BuildMeshDrawCommands(const Scene& InScene, bool AsyncBuilding)
{
	for (auto& MeshDrawCmds : m_MeshDrawCommands)
	{
		MeshDrawCmds.clear();
		MeshDrawCmds.reserve(1024u);
	}

	if (AsyncBuilding)
	{
		assert(false);
	}
	else
	{
		for (auto Node : InScene.GetVisibleNodes())
		{
			if (!Node)
			{
				continue;
			}

			if (Node->IsStaticMesh())
			{
				if (auto Mesh = InScene.GetStaticMesh(Node->GetDataIndex()))
				{
					for (uint32_t Index = 0u; Index < (uint32_t)EGeometryPassFilter::Num; ++Index)
					{
						m_MeshDrawCommands[Index].emplace_back(MeshDrawCommand(
							*Mesh, 
							InScene.GetMaterial(Mesh->GetMaterialID().GetIndex()), 
							m_PipelineBuilders[Index].get()));
					}
				}
			}
		}
	}
}

const std::vector<std::shared_ptr<IView>>& RenderScene::GetViews() const
{
	return m_Views;
}