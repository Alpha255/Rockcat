#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/PreDepthPass.h"

void PreDepthPass::Compile()
{
	AddInput("SceneDepth").CreateAsImage()
		.SetWidth(0u)
		.SetHeight(0u)
		.SetFormat(ERHIFormat::D32_Float_S8_UInt)
		.SetUsages(ERHIBufferUsageFlags::DepthStencil);
}
