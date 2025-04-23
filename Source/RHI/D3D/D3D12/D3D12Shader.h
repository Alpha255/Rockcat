#pragma once

#include "RHI/RHIShader.h"
#include "RHI/D3D/D3D12/D3D12Types.h"

class D3D12InputLayout : public RHIInputLayout
{
public:
	D3D12InputLayout(const RHIInputLayoutCreateInfo& RHICreateInfo);

	const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetInputElements() const { return m_Elements; }
protected:
private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_Elements;
};

class D3D12Shader : public RHIShader
{
public:
	D3D12Shader(const RHIShaderCreateInfo& RHICreateInfo);
};
