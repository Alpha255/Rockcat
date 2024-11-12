#include "Engine/Asset/ShaderAsset.h"
#include "Engine/RHI/RHIPipeline.h"

RHIGraphicsPipelineCreateInfo& RHIGraphicsPipelineCreateInfo::SetShader(const ShaderAsset& Shader, ERenderHardwareInterface Interface)
{
	auto Index = static_cast<uint32_t>(Shader.GetStage());
	Shaders[Index].SetStage(Shader.GetStage())
		.SetShaderBinary(Shader.GetBinary(Interface))
		.SetName(Shader.GetName().string());

	return *this;
}
