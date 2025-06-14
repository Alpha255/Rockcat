#include "Rendering/RenderGraph/RenderScene.h"
#include "Scene/Scene.h"
#include "Scene/SceneVisitor.h"
#include "Services/TaskFlowService.h"
#include "RHI/RHIDevice.h"
#include "Rendering/RenderGraph/RenderPass/GeometryPass.h"

MeshDrawCommand::MeshDrawCommand(const StaticMesh& Mesh)
	: IndexBuffer(Mesh.GetIndexBuffer())
	//, Material(&Mesh.GetMaterialProperty())
	, FirstIndex(0u)
	, NumPrimitives(Mesh.GetNumPrimitive())
	, NumIndex(Mesh.GetNumIndex())
	, IndexFormat(Mesh.GetIndexFormat())
{
	VertexArgs.BaseIndex = 0;
	VertexArgs.NumVertex = Mesh.GetNumVertex();
}

Array<std::unique_ptr<MeshDrawCommandBuilder>, EGeometryPass> RenderScene::s_Builders;

RenderScene::RenderScene(const Scene& InScene, bool AsyncMeshDrawCommandsBuilding)
	: m_Scene(InScene)
	, m_AsyncMeshDrawCommandsBuilding(AsyncMeshDrawCommandsBuilding)
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
#if 0
	if (auto Mesh = m_Scene.GetStaticMesh(MeshIndex))
	{
		auto It = m_MeshBatch[MeshIndex].find(Mesh->GetMaterialID());
		if (It == m_MeshBatch[MeshIndex].end())
		{
			It = m_MeshBatch[MeshIndex].emplace(Mesh->GetMaterialID(), 0u).first;
		}
		It->second += Add;
	}
#endif
}

void RenderScene::GetScenePrimitives()
{
#if 0
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
#endif
}

void RenderScene::UpdateScenePrimitives()
{
#if 0
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
#endif
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

void RenderScene::BuildMeshDrawCommands(const ISceneView& SceneView)
{
#if 0
	UpdateScenePrimitives();
	RemoveInvalidCommands();

	if (m_Primitives.Add.empty())
	{
		return;
	}

	if (m_AsyncMeshDrawCommandsBuilding)
	{
		m_CommandsEvent = tf::ParallelFor(m_Primitives.Add.begin(), m_Primitives.Add.end(), [this, &SceneView](const EntityID& ID) {
			if (auto Mesh = m_Scene.GetStaticMesh(ID))
			{
				for (size_t Index = 0u; Index < (size_t)EGeometryPass::Num; ++Index)
				{
					if (auto Builder = GetBuilder(Index))
					{
						auto Command = Builder->Build(*Mesh, SceneView);

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
				for (size_t Index = 0u; Index < (size_t)EGeometryPass::Num; ++Index)
				{
					if (auto Builder = GetBuilder(Index))
					{
						m_NodeIDCommandMap[ID] = m_Commands[Index].size();
						m_Commands[Index].emplace_back(Builder->Build(*Mesh, SceneView));
					}
				}
			}
		}
	}

	m_Primitives.Clear();
#endif
}
