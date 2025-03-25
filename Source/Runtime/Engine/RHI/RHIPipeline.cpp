#include "Engine/Asset/Shader.h"
#include "Engine/RHI/RHIPipeline.h"

RHIPipelineState::RHIPipelineState(const RHIGraphicsPipelineCreateInfo& GfxPipelineCreateInfo)
{
	for (auto& Shader : GfxPipelineCreateInfo.ShaderPipeline)
	{
		if (!Shader)
		{
			continue;
		}

		auto& Bindings = m_ShaderResourceLayout[Shader->GetStage()];
		std::vector<RHIShaderResourceBinding> AllBindings;
		uint32_t MaxBindingIndex = 0u;
		for (auto& [Name, Variable] : Shader->GetVariables())
		{
			RHIShaderResourceBinding Binding
			{
				Variable.Binding,
				0u,
				Variable.Type
			};

			MaxBindingIndex = std::max(MaxBindingIndex, Variable.Binding);
			AllBindings.emplace_back(std::move(Binding));
		}

		Bindings.clear();
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
		CommitPipelineStates(CommandBuffer);
		CommitShaderResources(CommandBuffer);
		ClearDirty();
	}
}

size_t RHIGraphicsShaderPipeline::ComputeHash() const
{
	size_t Hash = 0u;
	for (auto& Shader : *this)
	{
		if (Shader)
		{
			HashCombine(Hash, Shader->TryGetRHI());
		}
	}
	return Hash;
}
