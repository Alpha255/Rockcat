#if 0
#include "Colorful/IRenderer/RenderGraph/IFrameGraph.h"
#include "Colorful/IRenderer/IRenderer.h"
#include "Runtime/Scene/Scene.h"

NAMESPACE_START(RHI)

IFrameGraph::IFrameGraph(ERenderingPath RenderingPath, IFrameGraph*)
	: m_RenderingPath(RenderingPath)
{
	m_Device = IRenderer::Get().Device();
}

void IFrameGraphPass::Draw(ICommandBuffer* Command, const Drawable& DstDrawable)
{
	assert(Command);

	if (DstDrawable.Mesh)
	{
		/// Profiler::ScopedDebugMarker(Command, Drawable.Mesh->GetMesh()->debugName(), Color::random());

		SetupMaterial(DstDrawable.Mesh);

		assert(m_GraphicsPipeline);

		if (auto state = m_GraphicsPipeline->State())
		{
			state->SetVertexBuffer(DstDrawable.Mesh->GetMesh()->VertexBuffer());

			state->SetIndexBuffer(DstDrawable.Mesh->GetMesh()->IndexBuffer(), DstDrawable.Mesh->GetMesh()->IndexFormat());

			state->SetViewport(m_Viewport);

			state->SetScissorRect(m_Scissor);

			Command->SetGraphicsPipeline(m_GraphicsPipeline.get());

			UpdateUniformBuffers(Command);

			/// #Temp
			if (DstDrawable.m_Instances.size() > 1u)
			{
				assert(false);
				Command->DrawIndexedInstanced(
					DstDrawable.Mesh->GetMesh()->NumIndex(),
					static_cast<uint32_t>(DstDrawable.m_Instances.size()),
					0u,
					0);
			}
			else
			{
				Command->DrawIndexed(DstDrawable.Mesh->GetMesh()->NumIndex(), 0u, 0);
			}
		}
	}
}

NAMESPACE_END(RHI)

#endif