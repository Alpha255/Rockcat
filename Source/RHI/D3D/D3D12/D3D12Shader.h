#pragma once

#include "Runtime/Engine/RHI/RHIShader.h"
#include "RHI/D3D/D3D12/D3D12Types.h"

class D3D12InputLayout : public RHIInputLayout
{
public:
	D3D12InputLayout(const RHIInputLayoutCreateInfo& RHICreateInfo);

	const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputElementDescs() const { return m_ElementDescs; }
protected:
private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_ElementDescs;
};

class D3D12Shader : public RHIShader
{
public:
	D3D12Shader(const RHIShaderCreateInfo& RHICreateInfo);
};
