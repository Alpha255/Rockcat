#include "Engine/Rendering/RenderGraph/RenderScene.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneVisitor.h"
#include "Engine/Services/TaskFlowService.h"
#include "Engine/RHI/RHIDevice.h"
#include "Engine/Rendering/RenderGraph/RenderPass/GeometryPass.h"

MeshDrawCommand::MeshDrawCommand(const StaticMesh& Mesh)
	: IndexBuffer(Mesh.GetIndexBuffer())
	, Material(&Mesh.GetMaterialProperty())
	, FirstIndex(0u)
	, NumPrimitives(Mesh.GetNumPrimitive())
	, NumIndex(Mesh.GetNumIndex())
	, IndexFormat(Mesh.GetIndexFormat())
{
	VertexArgs.BaseIndex = 0;
	VertexArgs.NumVertex = Mesh.GetNumVertex();
}

Array<std::unique_ptr<MeshDrawCommandBuilder>, EGeometryPass> RenderScene::s_Builders;

RenderScene::RenderScene(const Scene& InScene)
	: m_Scene(InScene)
{
	for (auto& Commands : m_Commands)
	{
		Commands.reserve(1024u);
	}

	GetScenePrimitives();
}

void RenderScene::RegisterMeshDrawCommandBuilder(EGeometryPass Filter, MeshDrawCommandBuilder* Builder)
{
	assert(!s_Builders[Filter]);
	s_Builders[Filter].reset(Builder);
}

void RenderScene::UpdateMeshBatch(uint32_t MeshIndex, int32_t Add)
{
	if (auto Mesh = m_Scene.GetStaticMesh(MeshIndex))
	{
		auto It = m_MeshBatch[MeshIndex].find(Mesh->GetMaterialID());
		if (It == m_MeshBatch[MeshIndex].end())
		{
			It = m_MeshBatch[MeshIndex].emplace(Mesh->GetMaterialID(), 0u).first;
		}
		It->second += Add;
	}
}

void RenderScene::GetScenePrimitives()
{
	if (m_Scene.GetNumPrimitives() > 0u)
	{
		m_Primitives.Add.reserve(m_Scene.GetNumPrimitives());
		m_Primitives.Remove.reserve(m_Scene.GetNumPrimitives());
		m_MeshBatch.resize(m_Scene.GetNumPrimitives());

		SceneVisitor<BreadthFirst> Traverser(m_Scene);
		auto It = Traverser.Get();
		while (auto Node = It._Unwrapped())
		{
			if (Node->IsPrimitive())
			{
				m_Primitives.Add.push_back(Node->GetID());
				//UpdateMeshBatch(Node->GetDataIndex(), 1);
			}

			It = Traverser.Next();
		}
	}
}

void RenderScene::UpdateScenePrimitives()
{
	for (auto& ID : m_Scene.GetAddNodes())
	{
		if (auto Node = m_Scene.GetNode(ID))
		{
			if (Node->IsPrimitive())
			{
				m_Primitives.Add.push_back(ID);
				//UpdateMeshBatch(Node->GetDataIndex(), 1);
			}
		}
	}

	for (auto& Node : m_Scene.GetRemovedNodes())
	{
		if (Node.IsPrimitive())
		{
			m_Primitives.Remove.push_back(Node.GetID());
			//UpdateMeshBatch(Node.GetDataIndex(), -1);
		}
	}
}

void RenderScene::RemoveInvalidCommands()
{
	for (auto& ID : m_Primitives.Remove)
	{
		if (auto It = m_NodeIDCommandMap.find(ID); It != m_NodeIDCommandMap.end())
		{
			for (auto& Commands : m_Commands)
			{
				Commands.erase(Commands.begin() + It->second);
			}
		}
	}
}

void RenderScene::WaitCommandsBuilding()
{
	if (m_CommandsEvent)
	{
		m_CommandsEvent->Wait();
	}
}

void RenderScene::BuildMeshDrawCommands(RHIDevice& Device, const RenderSettings& GraphicsSettings)
{
	UpdateScenePrimitives();
	RemoveInvalidCommands();

	if (m_Primitives.Add.empty())
	{
		return;
	}

	if (Device.SupportsAsyncMeshDrawCommandsBuilding())
	{
		m_CommandsEvent = tf::ParallelFor(m_Primitives.Add.begin(), m_Primitives.Add.end(), [this, &GraphicsSettings, &Device](const SceneGraph::NodeID& ID) {
			if (auto Mesh = m_Scene.GetStaticMesh(ID))
			{
				const_cast<StaticMesh*>(Mesh)->CreateRHI(Device);

				for (size_t Index = 0u; Index < (size_t)EGeometryPass::Num; ++Index)
				{
					if (auto Builder = GetBuilder(Index))
					{
						auto Command = Builder->Build(*Mesh, GraphicsSettings);

						std::lock_guard<std::mutex> Lock(m_CommandsLock);
						m_NodeIDCommandMap[ID] = m_Commands[Index].size();
						m_Commands[Index].emplace_back(std::move(Command));
					}
				}
			}
		});
	}
	else
	{
		for (auto& ID : m_Primitives.Add)
		{
			if (auto Mesh = m_Scene.GetStaticMesh(ID))
			{
				const_cast<StaticMesh*>(Mesh)->CreateRHI(Device);

				for (size_t Index = 0u; Index < (size_t)EGeometryPass::Num; ++Index)
				{
					if (auto Builder = GetBuilder(Index))
					{
						m_NodeIDCommandMap[ID] = m_Commands[Index].size();
						m_Commands[Index].emplace_back(Builder->Build(*Mesh, GraphicsSettings));
					}
				}
			}
		}
	}

	m_Primitives.Clear();
}
