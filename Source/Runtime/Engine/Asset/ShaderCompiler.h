#pragma once

#include "RHI/D3D/DXGI_Interface.h"
#include "Runtime/Engine/RHI/RHIShader.h"
#include "Runtime/Engine/Application/GraphicsSettings.h"
#include <dxc/dxcapi.h>

class IShaderCompiler
{
public:
	virtual std::shared_ptr<RHIShaderCreateInfo> Compile(
		const char8_t* SourceName,
		const char8_t* SourceCode,
		size_t SourceCodeSize,
		const char8_t* ShaderEntryName,
		ERHIShaderStage ShaderStage,
		const class ShaderDefinitions& Definitions) = 0;

	static const char8_t* const GetShaderModelName(ERHIShaderStage ShaderState, bool8_t DXC)
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

	static std::shared_ptr<IShaderCompiler> Create(ERenderHardwareInterface RHI);
protected:
	virtual void GetShaderReflection(const class ShaderBinary& Binary) = 0;
};

class DxcShaderCompiler : public IShaderCompiler
{
public:
	DxcShaderCompiler(bool8_t GenerateSpirv);

	std::shared_ptr<RHIShaderCreateInfo> Compile(
		const char8_t* SourceName,
		const char8_t* SourceCode,
		size_t SourceCodeSize,
		const char8_t* ShaderEntryName,
		ERHIShaderStage ShaderStage,
		const class ShaderDefinitions& Definitions) override final;
protected:
	using DxcCompiler = D3DHwResource<IDxcCompiler3>;
	using DxcBlobEncoding = D3DHwResource<IDxcBlobEncoding>;
	using DxcResult = D3DHwResource<IDxcResult>;
	using DxcBlob = D3DHwResource<IDxcBlob>;
	using DxcUtils = D3DHwResource<IDxcUtils>;
	using DxcCompilerArgs = D3DHwResource<IDxcCompilerArgs>;
	using DxcIncludeHandler = D3DHwResource<IDxcIncludeHandler>;
	using DxcContainerReflection = D3DHwResource<IDxcContainerReflection>;
	using D3D12LibraryReflection = D3DHwResource<ID3D12LibraryReflection>;
	using D3D12ShaderReflection = D3DHwResource<ID3D12ShaderReflection>;

	void GetShaderReflection(const class ShaderBinary& Binary) override final;
private:
	DxcUtils m_Utils;
	DxcCompiler m_Compiler;
	DxcBlobEncoding m_Blob;
	bool8_t m_GenSpirv;
};

class D3DShaderCompiler : public IShaderCompiler
{
public:
	std::shared_ptr<RHIShaderCreateInfo> Compile(
		const char8_t* SourceName,
		const char8_t* SourceCode,
		size_t SourceCodeSize,
		const char8_t* ShaderEntryName,
		ERHIShaderStage ShaderStage,
		const class ShaderDefinitions& Definitions) override final;
protected:
	using D3D11ShaderReflection = D3DHwResource<ID3D11ShaderReflection>;

	void GetShaderReflection(const class ShaderBinary& Binary) override final;
};

#if 0

class VkShaderCompiler : public DxcShaderCompiler
{
public:
	std::shared_ptr<RHI::ShaderDesc> Compile(
		const char8_t* SourceName,
		const void* Source,
		size_t Size,
		const char8_t* Entry,
		RHI::EShaderStage Stage,
		const std::vector<RHI::ShaderMacro>& Macros) override final;
protected:
	void GetReflectionInfo(RHI::ShaderDesc* const Desc) override final;

	enum EVersion
	{
		OGL_ES = 100,
		OGL_Desktop = 110
	};

	static EShLanguage ShLang(RHI::EShaderStage Stage)
	{
		switch (Stage)
		{
		case RHI::EShaderStage::Vertex:   return EShLangVertex;
		case RHI::EShaderStage::Hull:     return EShLangTessControl;
		case RHI::EShaderStage::Domain:   return EShLangTessEvaluation;
		case RHI::EShaderStage::Geometry: return EShLangGeometry;
		case RHI::EShaderStage::Fragment: return EShLangFragment;
		case RHI::EShaderStage::Compute:  return EShLangCompute;
		}

		return EShLangCount;
	}
};
#endif