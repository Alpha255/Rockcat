#include "Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"
#include "Engine/RHI/RHIInterface.h"
#include "Engine/RHI/RHIDevice.h"
#include "Engine/Scene/Components/StaticMesh.h"
#include "Engine/Async/Task.h"

class MeshDrawTask : public Task
{
public:
	MeshDrawTask(const MeshDrawCommand* Command, RHICommandBuffer* CommandBuffer)
		: Task("MeshDrawTask", Task::ETaskType::Graphics, Task::EPriority::High)
		, m_DrawCommand(Command)
		, m_CommandBuffer(CommandBuffer)
	{
		assert(m_DrawCommand);
	}

	void Execute() override final
	{
		auto& Mesh = *m_DrawCommand->Mesh;
		m_CommandBuffer->SetVertexBuffer(Mesh.GetVertexBuffer(), 0u, 0u);
		m_CommandBuffer->SetIndexBuffer(Mesh.GetIndexBuffer(), 0u, Mesh.GetIndexFormat());
		m_CommandBuffer->DrawIndexedInstanced(Mesh.GetNumIndex(), m_DrawCommand->NumInstances, 0u, 0, 0u);
	}
private:
	const MeshDrawCommand* m_DrawCommand;
	RHICommandBuffer* m_CommandBuffer;
};

void MeshPass::Execute(RHIDevice& Device, const RenderScene& Scene)
{
	auto CommandBuffer = Device.GetActiveCommandBuffer(ERHIDeviceQueue::Graphics);
	//assert(CommandBuffer);

	if (GetRHI().GetGraphicsSettings().AsyncCommandlistSubmission)
	{

	}
	else
	{
		for (auto DrawCommand : Scene.GetDrawCommands(m_Filter))
		{
			MeshDrawTask(DrawCommand, CommandBuffer.get()).Execute();
		}
	}

	//CommandBuffer->Begin();
	//CommandBuffer->End();
}
