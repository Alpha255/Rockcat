#include "Engine/Rendering/RenderGraph/RenderPass/PreDepthPass.h"
#include "Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Engine/RHI/RHIInterface.h"
#include "Engine/Asset/GlobalShaders/DefaultShading.h"

void PreDepthPass::Compile()
{
	auto& GfxSettings = RHIInterface::GetGraphicsSettings();

	AddOutput("SceneDepth").CreateAsTexture()
		.SetWidth(GfxSettings.Resolution.Width)
		.SetHeight(GfxSettings.Resolution.Height)
		.SetDimension(ERHITextureDimension::T_2D)
		.SetFormat(ERHIFormat::D32_Float_S8_UInt)
		.SetUsages(ERHIBufferUsageFlags::DepthStencil);

	auto DepthStencilState = RHIDepthStencilStateCreateInfo()
		.SetEnableDepth(true)
		.SetEnableDepthWrite(true)
		.SetDepthCompareFunc(GfxSettings.InverseDepth ? ERHICompareFunc::LessOrEqual : ERHICompareFunc::GreaterOrEqual);

	auto GraphicsPipelineDesc = RHIGraphicsPipelineCreateInfo()
		.SetDepthStencilState(DepthStencilState)
		.SetShader(GenericVS(), GfxSettings.RenderHardwareInterface)
		.SetShader(DepthOnly(), GfxSettings.RenderHardwareInterface);
}
