#include "Engine/Asset/ShaderAsset.h"
#include "Engine/RHI/RHIPipeline.h"

RHIPipelineState::RHIPipelineState(const RHIGraphicsPipelineCreateInfo& GfxPipelineCreateInfo)
{
	for (auto Shader : GfxPipelineCreateInfo.Shaders)
	{
		if (!Shader)
		{
			continue;
		}

		auto& Bindings = m_ShaderResourceLayout[static_cast<size_t>(Shader->GetStage())];
		std::vector<RHIShaderResourceBinding> AllBindings;
		uint32_t MaxBindingIndex = 0u;
		for (auto& [Name, Variable] : Shader->GetVariables())
		{

		}

		Bindings.resize(MaxBindingIndex + 1u);
		for (auto& ResourceBinding : AllBindings)
		{
			Bindings[ResourceBinding.Binding] = ResourceBinding;
		}
	}
}

void RHIPipelineState::Commit(RHICommandBuffer* CommandBuffer)
{
	assert(CommandBuffer);

	if (IsDirty())
	{
		CommitShaderResources();
		ClearDirty();
	}
}
