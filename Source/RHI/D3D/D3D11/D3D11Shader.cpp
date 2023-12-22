#include "RHI/D3D/D3D11/D3D11Shader.h"
#include "RHI/D3D/D3D11/D3D11Device.h"
#include "Runtime/Engine/Services/SpdLogService.h"

D3D11VertexShader::D3D11VertexShader(const D3D11Device& Device, const RHIShaderCreateInfo& RHICreateInfo)
{
}

D3D11HullShader::D3D11HullShader(const D3D11Device& Device, const RHIShaderCreateInfo& RHICreateInfo)
{
}

D3D11DomainShader::D3D11DomainShader(const D3D11Device& Device, const RHIShaderCreateInfo& RHICreateInfo)
{
}

D3D11GeometryShader::D3D11GeometryShader(const D3D11Device& Device, const RHIShaderCreateInfo& RHICreateInfo)
{
}

D3D11ComputeShader::D3D11ComputeShader(const D3D11Device& Device, const RHIShaderCreateInfo& RHICreateInfo)
{
}

D3D11FragmentShader::D3D11FragmentShader(const D3D11Device& Device, const RHIShaderCreateInfo& RHICreateInfo)
{
}

D3D11InputLayout::D3D11InputLayout(const D3D11Device& Device, const RHIInputLayoutCreateInfo& RHICreateInfo)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
	for (uint32_t i = 0u; i < desc.Bindings.size(); ++i)
	{
		for (uint32_t j = 0u; j < desc.Bindings[i].Attributes.size(); ++j)
		{
			assert(desc.Bindings[i].Binding < ELimitations::MaxVertexStreams);

			auto& layout = desc.Bindings[i].Attributes[j];
			D3D11_INPUT_ELEMENT_DESC inputElement
			{
				shaderDesc.Language == EShaderLanguage::HLSL ? layout.Usage.c_str() : "TEXCOORD",
				shaderDesc.Language == EShaderLanguage::HLSL ? 0u : j,
				D3D11EnumTranslator::format(layout.Format),
				desc.Bindings[i].Binding,
				D3D11_APPEND_ALIGNED_ELEMENT,  /// Define the current element directly after the previous one, including any packing if necessary
				desc.Bindings[i].InputRate == EVertexInputRate::Instance ?
					D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA,
				desc.Bindings[i].InputRate == EVertexInputRate::Instance ? 1u : 0u  /// ??? 
			};
			inputElements.emplace_back(std::move(inputElement));
		}
	}

	VERIFY_D3D(device->CreateInputLayout(
		inputElements.data(),
		static_cast<uint32_t>(inputElements.size()),
		shaderDesc.Binary.get(),
		shaderDesc.BinarySize,
		reference()));
}