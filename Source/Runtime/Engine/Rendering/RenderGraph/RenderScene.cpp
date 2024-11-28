#include "Engine/Rendering/RenderGraph/RenderScene.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/GlobalShaders/DefaultShading.h"
#include "Engine/Services/RenderService.h"
#include "Engine/Rendering/RenderGraph/RenderPass/GeometryPass.h"

MeshDrawCommand::MeshDrawCommand(const StaticMesh& Mesh)
	: IndexBuffer(Mesh.GetIndexBuffer())
	, Material(Mesh.GetMaterialID())
	, FirstIndex(0u)
	, NumPrimitives(Mesh.GetNumPrimitive())
	, NumIndex(Mesh.GetNumIndex())
	, PrimitiveTopology(Mesh.GetPrimitiveTopology())
	, IndexFormat(Mesh.GetIndexFormat())
{
	VertexArgs.BaseIndex = 0u;
	VertexArgs.NumVertex = Mesh.GetNumVertex();
}

void RenderScene::RegisterMeshDrawCommandBuilder(EGeometryPassFilter Filter, IGeometryPassMeshDrawCommandBuilder* Builder)
{
	m_MeshDrawCommandBuilders[static_cast<size_t>(Filter)].reset(Builder);
}

void RenderScene::BuildMeshDrawCommands(const Scene& InScene, RHIDevice& Device, bool Async)
{
	for (auto& MeshDrawCmds : m_MeshDrawCommands)
	{
		MeshDrawCmds.clear();
		MeshDrawCmds.reserve(1024u);
	}

	if (Async)
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
					const_cast<StaticMesh*>(Mesh)->CreateRHIBuffers(Device);

					for (uint32_t Index = 0u; Index < (uint32_t)EGeometryPassFilter::Num; ++Index)
					{
						m_MeshDrawCommands[Index].emplace_back(m_MeshDrawCommandBuilders[Index]->Build(*Mesh, InScene));
					}
				}
			}
			else if (Node->IsSkeletalMesh())
			{
				assert(false);
			}
		}
	}
}
