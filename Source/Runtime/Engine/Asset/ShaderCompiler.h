#pragma once

#include "RHI/D3D/DXGIInterface.h"
#include "Engine/RHI/RHIShader.h"
#include "Engine/Application/GraphicsSettings.h"
#include "Engine/Asset/Shader.h"
#include <dxc/dxcapi.h>

class IShaderCompiler
{
public:
	IShaderCompiler(ERHIBackend Backend)
		: m_Backend(Backend)
	{
	}

	ERHIBackend GetBackend() const { return m_Backend; }

	virtual ShaderBlob Compile(
		const char* SourceName,
		const char* SourceCode,
		size_t SourceCodeSize,
		const char* EntryPoint,
		ERHIShaderStage ShaderStage,
		const class ShaderDefines& Definitions) = 0;
protected:
	virtual void GetShaderReflection(const ShaderBlob& Blob) = 0;

	static const char* const GetProfile(ERHIShaderStage ShaderState, bool DXC)
	{
		switch (ShaderState)
		{
		case ERHIShaderStage::Vertex:   return DXC ? "vs_6_0" : "vs_5_0";
		case ERHIShaderStage::Hull:     return DXC ? "hs_6_0" : "hs_5_0";
		case ERHIShaderStage::Domain:   return DXC ? "ds_6_0" : "ds_5_0";
		case ERHIShaderStage::Geometry: return DXC ? "gs_6_0" : "gs_5_0";
		case ERHIShaderStage::Fragment: return DXC ? "ps_6_0" : "ps_5_0";
		case ERHIShaderStage::Compute:  return DXC ? "cs_6_0" : "cs_5_0";
		default:
			return nullptr;
		}
	}
private:
	ERHIBackend m_Backend = ERHIBackend::Num;
};

class DxcShaderCompiler : public IShaderCompiler
{
public:
	DxcShaderCompiler(ERHIBackend Backend, bool GenerateSpirv);

	ShaderBlob Compile(
		const char* SourceName,
		const char* SourceCode,
		size_t SourceCodeSize,
		const char* EntryPoint,
		ERHIShaderStage ShaderStage,
		const class ShaderDefines& Definitions) override final;
protected:
	using DxcCompiler = D3DHwResource<IDxcCompiler3>;
	using DxcBlobEncoding = D3DHwResource<IDxcBlobEncoding>;
	using DxcResult = D3DHwResource<IDxcResult>;
	using DxcBlob = D3DHwResource<IDxcBlob>;
	using DxcUtils = D3DHwResource<IDxcUtils>;
	using DxcCompilerArgs = D3DHwResource<IDxcCompilerArgs>;
	using DxcIncludeHandler = D3DHwResource<IDxcIncludeHandler>;
	using DxcContainerReflection = D3DHwResource<IDxcContainerReflection>;
	using DxcVersionInfo = D3DHwResource<IDxcVersionInfo>;

	using D3D12LibraryReflection = D3DHwResource<ID3D12LibraryReflection>;
	using D3D12ShaderReflection = D3DHwResource<ID3D12ShaderReflection>;

	void GetShaderReflection(const ShaderBlob&) override final {}
private:
	DxcUtils m_Utils;
	DxcCompiler m_Compiler;
	bool m_GenSpirv;
};

class D3DShaderCompiler : public IShaderCompiler
{
public:
	using IShaderCompiler::IShaderCompiler;

	ShaderBlob Compile(
		const char* SourceName,
		const char* SourceCode,
		size_t SourceCodeSize,
		const char* EntryPoint,
		ERHIShaderStage ShaderStage,
		const class ShaderDefines& Definitions) override final;
protected:
	using D3D11ShaderReflection = D3DHwResource<ID3D11ShaderReflection>;

	void GetShaderReflection(const ShaderBlob&) override final {}
};