#include "Rendering/RenderGraph/RenderPass/GeometryPass.h"
#include "RHI/RHIDevice.h"
#include "RHI/RHICommandListContext.h"
#include "Scene/Components/StaticMesh.h"
#include "Rendering/RenderScene.h"
#include "Rendering/RenderGraph/RenderGraph.h"
#include "Async/Task.h"

struct ScopeDebugMarker
{
	ScopeDebugMarker(RHICommandBuffer* InCommandBuffer, const std::string_view& MarkerName, const Math::Color& MarkerColor)
		: ShouldMarker(!MarkerName.empty())
		, CommandBuffer(InCommandBuffer)
	{
		assert(InCommandBuffer);

		if (ShouldMarker)
		{
			CommandBuffer->BeginDebugMarker(MarkerName.data(), MarkerColor);
		}
	}

	~ScopeDebugMarker()
	{
		if (ShouldMarker)
		{
			CommandBuffer->EndDebugMarker();
		}
	}

	bool ShouldMarker = false;
	RHICommandBuffer* CommandBuffer = nullptr;
};

struct MeshDrawTask : public Task
{
	MeshDrawTask(const MeshDrawCommand& Command, RHICommandListContext* CommandListContext)
		: Task("MeshDrawTask", EPriority::High)
		, DrawCommand(Command)
		, CommandBuffer(CommandListContext->GetGraphicsCommandBuffer())
	{
		assert(CommandBuffer);
	}

	const MeshDrawCommand& DrawCommand;
	RHICommandBuffer* CommandBuffer;

protected:
	void ExecuteImpl() override final
	{
		/// How to update shader resources ??? 
		/// How to update uniform buffer ???
		//auto PipelineState = DrawCommand.GraphicsPipeline->GetPipelineState();
		//assert(PipelineState);

		ScopeDebugMarker DebugMarker(CommandBuffer, DrawCommand.GetDebugName(), Math::Color::Random());

		//CommandBuffer->SetGraphicsPipeline(DrawCommand.GraphicsPipeline);
		CommandBuffer->SetVertexStream(0u, DrawCommand.VertexStream);
		CommandBuffer->SetIndexBuffer(
			DrawCommand.IndexBuffer,
			DrawCommand.FirstIndex * static_cast<size_t>(DrawCommand.IndexFormat),
			DrawCommand.IndexFormat);
		CommandBuffer->DrawIndexedInstanced(DrawCommand.NumIndex, DrawCommand.NumInstances, DrawCommand.FirstIndex, DrawCommand.VertexArgs.BaseIndex);
	}
};

void MeshDrawCommandBuilder::SetupShaderParameters(RHIGraphicsPipelineDesc& Desc,
	const ISceneView& InView,
	const Math::Transform& InTransform,
	const MaterialProperty& InMaterial)
{
	//for (auto& Shader : Desc.ShaderPipeline)
	//{
	//	if (Shader)
	//	{
	//		Shader->SetupTransform(InTransform);
	//		Shader->SetupViewParams(InView);
	//		Shader->SetupMaterialProperty(InMaterial);
	//	}
	//}
}

RHIFrameBuffer* GeometryPass::GetFrameBuffer()
{
	//if (!m_FrameBuffer)
	//{
	//	RHIFrameBufferDesc CreateInfo;
	//	for (auto Resource : GetOutputs())
	//	{
	//		if (Resource && Resource->GetType() == RDGResource::EType::Texture)
	//		{
	//			CreateInfo.AddAttachment(Resource->GetRHI<RHITexture>());
	//		}
	//	}

	//	m_FrameBuffer = GetRenderDevice().GetOrCreateFrameBuffer(CreateInfo);
	//}
	
	return m_FrameBuffer;
}

void GeometryPass::Execute(const RenderScene& Scene)
{
	//for (auto& View : Scene.GetViews())
	//{
	//	if (GetRenderDevice().SupportsAsyncCommandlistSubmission())
	//	{
	//		assert(false);
	//	}
	//	else
	//	{
	//		//auto CommandListContext = GetDevice().GetImmediateCommandListContext(ERHIDeviceQueue::Graphics);
	//		//for (auto& DrawCommand : Scene.GetCommands())
	//		//{
	//		//	MeshDrawTask(DrawCommand, CommandListContext).Execute();
	//		//}
	//	}
	//}
}
