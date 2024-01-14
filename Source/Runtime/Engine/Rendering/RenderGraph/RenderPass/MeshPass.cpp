#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"
#include "Runtime/Engine/RHI/RHIDevice.h"
#include "Runtime/Engine/Scene/Components/StaticMesh.h"
#include "Runtime/Engine/Async/Task.h"

class MeshDrawTask : public Task
{
public:
	MeshDrawTask(const MeshDrawCommand* Command)
		: Task("MeshDrawTask", Task::ETaskType::Graphics, Task::EPriority::High)
		, m_Command(Command)
	{
		assert(m_Command);
	}

	void DoTask() override final
	{
	}
private:
	const MeshDrawCommand* m_Command;
};

void MeshPass::Execute(RHIDevice& Device, const RenderScene& Scene)
{
	for (auto DrawCommand : Scene.GetDrawCommands(m_Filter))
	{

	}
}
