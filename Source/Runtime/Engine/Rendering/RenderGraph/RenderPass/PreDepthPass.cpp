#include "Engine/Rendering/RenderGraph/RenderPass/PreDepthPass.h"
#include "Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Engine/RHI/RHIInterface.h"
#include "Engine/Asset/GlobalShaders/DefaultShading.h"

void PreDepthPass::Compile()
{
	auto& GraphicsSettings = RHIInterface::GetGraphicsSettings();

	AddOutput("SceneDepth").CreateAsImage()
		.SetWidth(GraphicsSettings.Resolution.Width)
		.SetHeight(GraphicsSettings.Resolution.Height)
		.SetImageType(ERHIImageType::T_2D)
		.SetFormat(ERHIFormat::D32_Float_S8_UInt)
		.SetUsages(ERHIBufferUsageFlags::DepthStencil);

	RHIGraphicsPipelineCreateInfo CreateInfo;
	CreateInfo.DepthStencilState.SetEnableDepth(true)
		.SetEnableDepthWrite(true)
		.SetDepthCompareFunc(GraphicsSettings.InverseDepth ? ERHICompareFunc::LessOrEqual : ERHICompareFunc::GreaterOrEqual);
	CreateInfo.SetShader(GenericVS().GetRHICreateInfo())
		.SetShader(DepthOnlyFS().GetRHICreateInfo());

	m_GraphicsPipeline = GetResourceManager().GetOrCreateGraphicsPipeline(CreateInfo);
}
