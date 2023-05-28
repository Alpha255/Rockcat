#if 0
#include "Colorful/IRenderer/IPipeline.h"

NAMESPACE_START(RHI)

PipelineState::PipelineState(const GraphicsPipelineDesc& Desc)
	: FrameBuffer(Desc.FrameBuffer)
{
	Desc.Shaders.ForEach([this](const IShader* Shader) {
		assert(Shader->Desc());
		for (auto& Variable : Shader->Desc()->Variables)
		{
			ShaderVariableTable.m_Variables.emplace_back(ShaderVariable(
				static_cast<uint32_t>(ShaderVariableTable.m_Variables.size()),
				Variable));
		}
	});
}

NAMESPACE_END(RHI)
#endif