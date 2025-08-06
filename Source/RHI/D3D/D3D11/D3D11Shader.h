#pragma once

#include "RHI/D3D/D3D11/D3D11Types.h"
#include "RHI/RHIShader.h"

class D3D11VertexShader final : public D3DHwResource<ID3D11VertexShader>, public RHIShader
{
public:
	D3D11VertexShader(const class D3D11Device& Device, const RHIShaderDesc& Desc);
};

class D3D11HullShader final : public D3DHwResource<ID3D11HullShader>, public RHIShader
{
public:
	D3D11HullShader(const class D3D11Device& Device, const RHIShaderDesc& Desc);
};

class D3D11DomainShader final : public D3DHwResource<ID3D11DomainShader>, public RHIShader
{
public:
	D3D11DomainShader(const class D3D11Device& Device, const RHIShaderDesc& Desc);
};

class D3D11GeometryShader final : public D3DHwResource<ID3D11GeometryShader>, public RHIShader
{
public:
	D3D11GeometryShader(const class D3D11Device& Device, const RHIShaderDesc& Desc);
};

class D3D11FragmentShader final : public D3DHwResource<ID3D11PixelShader>, public RHIShader
{
public:
	D3D11FragmentShader(const class D3D11Device& Device, const RHIShaderDesc& Desc);
};

class D3D11ComputeShader final : public D3DHwResource<ID3D11ComputeShader>, public RHIShader
{
public:
	D3D11ComputeShader(const class D3D11Device& Device, const RHIShaderDesc& Desc);
};

class D3D11InputLayout final : public D3DHwResource<ID3D11InputLayout>, public RHIInputLayout
{
public:
	D3D11InputLayout(const class D3D11Device& Device, const RHIInputLayoutDesc& Desc);
};