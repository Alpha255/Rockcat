#include "Engine/Rendering/RenderGraph/RenderPass/GeometryPass.h"
#include "Engine/RHI/RHIInterface.h"
#include "Engine/RHI/RHIDevice.h"
#include "Engine/RHI/RHICommandListContext.h"
#include "Engine/Scene/Components/StaticMesh.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Rendering/RenderGraph/RenderScene.h"
#include "Engine/Async/Task.h"

class MeshDrawTask : public Task
{
public:
	MeshDrawTask(const MeshDrawCommand* Command, RHICommandListContext* CommandListContext)
		: Task("MeshDrawTask", Task::EPriority::High)
		, m_DrawCommand(Command)
		, m_CommandBuffer(CommandListContext->GetCommandBuffer())
	{
		assert(m_DrawCommand && m_CommandBuffer);
	}

	void Execute() override final
	{
		//auto& Mesh = *m_DrawCommand->Mesh;
		//m_CommandBuffer->SetVertexBuffer(Mesh.GetVertexBuffer(), 0u, 0u);
		//m_CommandBuffer->SetIndexBuffer(Mesh.GetIndexBuffer(), 0u, Mesh.GetIndexFormat());
		//m_CommandBuffer->DrawIndexedInstanced(Mesh.GetNumIndex(), m_DrawCommand->NumInstances, 0u, 0, 0u);
	}
private:
	const MeshDrawCommand* m_DrawCommand;
	RHICommandBuffer* m_CommandBuffer;
};

GeometryPass::GeometryPass(DAGNodeID ID, const char* Name, RenderGraph& Graph, EGeometryPassFilter Filter, IGeometryPassMeshDrawCommandBuilder* MeshDrawCommandBuilder)
	: RenderPass(ID, Name, Graph)
	, m_Filter(Filter)
{
	assert(MeshDrawCommandBuilder);
	Graph.GetRenderScene().RegisterMeshDrawCommandBuilder(Filter, MeshDrawCommandBuilder);
}

void GeometryPass::Execute(RHIDevice& Device, const RenderScene& Scene)
{
	//if (GetRHI().GetGraphicsSettings().AsyncCommandlistSubmission)
	//{
	//	assert(false);
	//}
	//else
	//{
	//	auto RHICmdListContext = Device.GetImmediateCommandListContext(ERHIDeviceQueue::Graphics);
	//	for (auto DrawCommand : Scene.GetDrawCommands(m_Filter))
	//	{
	//		MeshDrawTask(DrawCommand, RHICmdListContext).Execute();
	//	}
	//}
}
