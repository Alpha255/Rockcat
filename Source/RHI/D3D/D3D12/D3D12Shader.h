#pragma once

#include "Colorful/IRenderer/IShader.h"
#include "Colorful/D3D/D3D12/D3D12Types.h"

NAMESPACE_START(RHI)

class D3D12InputLayout : public IInputLayout
{
public:
	D3D12InputLayout(const InputLayoutDesc& Desc);

	const std::vector<D3D12_INPUT_ELEMENT_DESC>& InputElementDescs() const
	{
		return m_ElementDescs;
	}
protected:
private:
	std::vector<D3D12_INPUT_ELEMENT_DESC> m_ElementDescs;
};

class D3D12Shader : public IShader
{
public:
	D3D12Shader(const ShaderDesc& Desc);
};

NAMESPACE_END(RHI)
