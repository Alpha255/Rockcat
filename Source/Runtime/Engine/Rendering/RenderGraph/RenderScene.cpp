#include "Engine/Rendering/RenderGraph/RenderScene.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Scene/SceneVisitor.h"
#include "Engine/Services/RenderService.h"
#include "Engine/Services/TaskFlowService.h"
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

RenderScene::RenderScene(const Scene& InScene)
	: m_Scene(InScene)
{
	for (auto& Commands : m_Commands)
	{
		Commands.reserve(1024u);
	}

	GetScenePrimitives();
}

void RenderScene::RegisterMeshDrawCommandBuilder(EGeometryPass Filter, IMeshDrawCommandBuilder* Builder)
{
	assert(!s_Builders[Filter]);
	s_Builders[Filter].reset(Builder);
}

void RenderScene::GetScenePrimitives()
{
	if (m_Scene.GetNumPrimitives() > 0u)
	{
		m_Primitives.Add.reserve(m_Scene.GetNumPrimitives());
		m_Primitives.Remove.reserve(m_Scene.GetNumPrimitives());

		SceneVisitor<BreadthFirst> Traverser(m_Scene);
		auto It = Traverser.Get();
		while (auto Node = It._Unwrapped())
		{
			if (Node->IsPrimitive())
			{
				m_Primitives.Add.push_back(Node->GetID());
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
			}
		}
	}

	for (auto& Node : m_Scene.GetRemovedNodes())
	{
		if (Node.IsPrimitive())
		{
			m_Primitives.Remove.push_back(Node.GetID());
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

void RenderScene::BuildMeshDrawCommands()
{
	UpdateScenePrimitives();
	RemoveInvalidCommands();

	if (RHIBackend::GetConfigs().AsyncMeshDrawCommandsBuilding)
	{
		if (m_Primitives.Add.empty())
		{
			return;
		}

		m_CommandsEvent = tf::ParallelFor(m_Primitives.Add.begin(), m_Primitives.Add.end(), [this](const SceneGraph::NodeID& ID) {
			if (auto Mesh = m_Scene.GetStaticMesh(ID))
			{
				for (size_t Index = 0u; Index < (size_t)EGeometryPass::Num; ++Index)
				{
					if (auto Builder = GetBuilder(Index))
					{
						auto Command = Builder->Build(*Mesh);

						std::lock_guard<std::mutex> Lock(m_CommandsMutex);
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
						m_Commands[Index].emplace_back(Builder->Build(*Mesh));
					}
				}
			}
		}
	}

	m_Primitives.Clear();
}
