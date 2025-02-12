#include "Engine/Rendering/RenderGraph/RenderScene.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Services/RenderService.h"
#include "Engine/Rendering/RenderGraph/RenderPass/GeometryPass.h"

MeshDrawCommand::MeshDrawCommand(const StaticMesh& Mesh)
	: IndexBuffer(Mesh.GetIndexBuffer())
	, MeshName(Mesh.GetName())
	, Material(Mesh.GetMaterialID())
	, FirstIndex(0u)
	, NumPrimitives(Mesh.GetNumPrimitive())
	, NumIndex(Mesh.GetNumIndex())
	, IndexFormat(Mesh.GetIndexFormat())
{
	VertexArgs.BaseIndex = 0;
	VertexArgs.NumVertex = Mesh.GetNumVertex();

	GraphicsPipelineDesc.SetPrimitiveTopology(Mesh.GetPrimitiveTopology());
}

Array<std::unique_ptr<IMeshDrawCommandBuilder>, EGeometryPass> RenderScene::s_Builders;

void RenderScene::RegisterMeshDrawCommandBuilder(EGeometryPass Filter, IMeshDrawCommandBuilder* Builder)
{
	assert(!s_Builders[Filter]);
	s_Builders[Filter].reset(Builder);
}

void RenderScene::TraverseScene()
{
}

void RenderScene::BuildMeshDrawCommands()
{
	for (auto& Commands : m_Commands)
	{
		Commands.clear();
		Commands.reserve(1024u);
	}

	if (RHIBackend::GetConfigs().AsyncMeshDrawCommandsBuilding)
	{
		assert(false);
	}
	else
	{
		//for (auto ID : m_Scene.GetVisibleNodes())
		//{
		//	if (!ID.IsValid())
		//	{
		//		continue;
		//	}

		//	//if (Node->IsStaticMesh())
		//	//{
		//	//	if (auto Mesh = m_Scene.GetStaticMesh(Node->GetDataIndex()))
		//	//	{
		//	//		///const_cast<StaticMesh*>(Mesh)->CreateRHIBuffers(Device);

		//	//		for (size_t Index = 0u; Index < (size_t)EGeometryPass::Num; ++Index)
		//	//		{
		//	//			if (auto Builder = GetBuilder(Index))
		//	//			{
		//	//				m_MeshDrawCommands[Index].emplace_back(Builder->Build(*Mesh));
		//	//			}
		//	//		}
		//	//	}
		//	//}
		//	//else if (Node->IsSkeletalMesh())
		//	//{
		//	//	assert(false);
		//	//}
		//}
	}
}
