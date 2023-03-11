#include "Colorful/D3D/D3D12/D3D12Shader.h"

NAMESPACE_START(RHI)

D3D12InputLayout::D3D12InputLayout(const InputLayoutDesc& Desc)
{
	uint32_t Offset = 0u;
	for (auto& Binding : Desc.Bindings)
	{
		for (auto& Attr : Binding.Attributes)
		{
			m_ElementDescs.emplace_back(
				D3D12_INPUT_ELEMENT_DESC 
				{
					Attr.Usage.c_str(),  /// #TODO Safety ??
					Binding.Binding,
					D3D12Type::Format(Attr.Format),
					Attr.Location,
					Offset,
					Binding.InputRate == EVertexInputRate::Instance ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
					Binding.InputRate == EVertexInputRate::Instance ? 1u : 0u
				});
			Offset += Attr.Stride * Binding.Binding;
		}
	}
}

D3D12Shader::D3D12Shader(const ShaderDesc& Desc)
	: IShader(Desc)
{
}

NAMESPACE_END(RHI)
