#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/MeshPass.h"
#include "Runtime/Engine/RHI/RHIDevice.h"
#include "Runtime/Engine/Scene/SceneView.h"
#include "Runtime/Engine/Scene/Components/StaticMesh.h"
#include "Runtime/Engine/Async/Task.h"

class MeshDrawTask : public Task
{
public:
	MeshDrawTask(const MeshPass::MeshDrawCommand* Command)
		: Task("MeshDrawTask", Task::ETaskType::Graphics, Task::EPriority::High)
		, m_Command(Command)
	{
		assert(m_Command);
	}

	void DoTask() override final
	{
	}
private:
	const MeshPass::MeshDrawCommand* m_Command;
};

void MeshPass::Execute(RHIDevice& RenderDevice, const Scene& RenderScene)
{
	GenerateMeshDrawCommands(RenderScene);
}

bool8_t MeshPass::NeedRecomputeSceneVisibility(const Scene& RenderScene) const
{
	return RenderScene.IsDirty() || s_Commands.empty();
}

void MeshPass::GenerateMeshDrawCommands(const Scene& RenderScene)
{
	// Compute dirty part only like UE5???
	if (!NeedRecomputeSceneVisibility(RenderScene))
	{
		return;
	}

	std::vector<MeshDrawCommand> MeshDrawCommands;
	std::unordered_map<const StaticMesh*, const MeshDrawCommand*> MeshCommandMap; // TODO: Do this when assimp scene loading?

	SceneView<BreadthFirst> SceneViewer(RenderScene);
	auto Node = SceneViewer.Get();
	while (Node != SceneNodeIterator())
	{
		if (!Node->IsAlive() || !Node->IsVisible())
		{
			SceneViewer.Next();
		}

		if (Node->IsStaticMesh())
		{
			MeshDrawCommand NewCommand;
			NewCommand.Mesh = RenderScene.GetStaticMesh(Node->GetDataIndex());
			NewCommand.WorldTransform = RenderScene.GetTransform(Node->GetDataIndex());
			NewCommand.IsSelected = Node->IsSelected();
			s_Commands.emplace_back(std::move(NewCommand));
		}

		if (Node->IsSkeletalMesh())
		{
			// TODO
		}
	}

	GenerateVisibleMeshDrawCommands(MeshDrawCommands);
}

void MeshPass::GenerateVisibleMeshDrawCommands(const std::vector<MeshDrawCommand>& MeshDrawCommands)
{
}
