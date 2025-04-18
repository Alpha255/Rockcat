#include "RHI/D3D/D3D11/D3D11Shader.h"
#include "RHI/D3D/D3D11/D3D11Device.h"

D3D11VertexShader::D3D11VertexShader(const D3D11Device& Device, const RHIShaderCreateInfo& RHICreateInfo)
	: RHIShader(RHICreateInfo)
{
	VERIFY_D3D(Device->CreateVertexShader(
		RHICreateInfo.Binary->GetBinary(),
		RHICreateInfo.Binary->GetSize(),
		nullptr,
		Reference()));
}

D3D11HullShader::D3D11HullShader(const D3D11Device& Device, const RHIShaderCreateInfo& RHICreateInfo)
	: RHIShader(RHICreateInfo)
{
	VERIFY_D3D(Device->CreateHullShader(
		RHICreateInfo.Binary->GetBinary(),
		RHICreateInfo.Binary->GetSize(),
		nullptr,
		Reference()));
}

D3D11DomainShader::D3D11DomainShader(const D3D11Device& Device, const RHIShaderCreateInfo& RHICreateInfo)
	: RHIShader(RHICreateInfo)
{
	VERIFY_D3D(Device->CreateDomainShader(
		RHICreateInfo.Binary->GetBinary(),
		RHICreateInfo.Binary->GetSize(),
		nullptr,
		Reference()));
}

D3D11GeometryShader::D3D11GeometryShader(const D3D11Device& Device, const RHIShaderCreateInfo& RHICreateInfo)
	: RHIShader(RHICreateInfo)
{
	VERIFY_D3D(Device->CreateGeometryShader(
		RHICreateInfo.Binary->GetBinary(),
		RHICreateInfo.Binary->GetSize(),
		nullptr,
		Reference()));
}

D3D11ComputeShader::D3D11ComputeShader(const D3D11Device& Device, const RHIShaderCreateInfo& RHICreateInfo)
	: RHIShader(RHICreateInfo)
{
	VERIFY_D3D(Device->CreateComputeShader(
		RHICreateInfo.Binary->GetBinary(),
		RHICreateInfo.Binary->GetSize(),
		nullptr,
		Reference()));
}

D3D11FragmentShader::D3D11FragmentShader(const D3D11Device& Device, const RHIShaderCreateInfo& RHICreateInfo)
	: RHIShader(RHICreateInfo)
{
	VERIFY_D3D(Device->CreatePixelShader(
		RHICreateInfo.Binary->GetBinary(),
		RHICreateInfo.Binary->GetSize(),
		nullptr,
		Reference()));
}

D3D11InputLayout::D3D11InputLayout(const D3D11Device& Device, const RHIInputLayoutCreateInfo& RHICreateInfo)
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> InputElements;
	for (uint32_t i = 0u; i < RHICreateInfo.Bindings.size(); ++i)
	{
		for (uint32_t j = 0u; j < RHICreateInfo.Bindings[i].Attributes.size(); ++j)
		{
			assert(RHICreateInfo.Bindings[i].Binding < ERHILimitations::MaxVertexStreams);

			auto& Layout = RHICreateInfo.Bindings[i].Attributes[j];
			D3D11_INPUT_ELEMENT_DESC InputElement
			{
#if 0
				shaderDesc.Language == EShaderLanguage::HLSL ? layout.Usage.c_str() : "TEXCOORD",
				shaderDesc.Language == EShaderLanguage::HLSL ? 0u : j,
				D3D11EnumTranslator::format(layout.Format),
				desc.Bindings[i].Binding,
				D3D11_APPEND_ALIGNED_ELEMENT,  /// Define the current element directly after the previous one, including any packing if necessary
				desc.Bindings[i].InputRate == EVertexInputRate::Instance ?
					D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA,
				desc.Bindings[i].InputRate == EVertexInputRate::Instance ? 1u : 0u  /// ??? 
#endif
			};
			InputElements.emplace_back(std::move(InputElement));
		}
	}

	VERIFY_D3D(Device->CreateInputLayout(
		InputElements.data(),
		static_cast<uint32_t>(InputElements.size()),
		nullptr,
		0u,
		Reference()));
}