#include "Colorful/IRenderer/RenderGraph/SimpleFrameGraph.h"
#include "Colorful/IRenderer/IRenderer.h"
#include "Runtime/Scene/Scene.h"

NAMESPACE_START(RHI)

void SimpleFrameGraph::Render(const RenderSettings* Settings, IFrameBuffer* SwapchainFrameBuffer)
{
	auto Command = m_Device->GetOrAllocateCommandBuffer(EQueueType::Graphics);

	for (auto& Pass : m_Passes)
	{
		Pass->SetSwapchainFrameBuffer(SwapchainFrameBuffer);

		Pass->ApplyRenderSettings(Settings);

		if (Pass->Enabled())
		{
			Pass->Render(Command.get());
		}
	}

	m_Device->SubmitCommandBuffer(EQueueType::Graphics, Command);
}

NAMESPACE_END(RHI)