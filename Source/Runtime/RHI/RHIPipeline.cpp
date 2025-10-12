#include "Asset/Shader.h"
#include "RHI/RHIPipeline.h"

RHIPipelineState::RHIPipelineState(const RHIGraphicsPipelineDesc& Desc)
{
	for (auto& Shader : Desc.Shaders)
	{
		if (!Shader)
		{
			continue;
		}

		for (auto& Variable : Shader->GetVariables())
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

RHIPipelineState::RHIPipelineState(const RHIComputePipelineDesc& Desc)
{
}
