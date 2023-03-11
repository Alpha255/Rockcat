#pragma once

#include "Colorful/IRenderer/IShader.h"
#include "Colorful/D3D/DXGI_Interface.h"
#include "Colorful/IRenderer/IRenderer.h"
#include <ThirdParty/glslang/glslang/Public/ShaderLang.h>
#include <ThirdParty/glslang/StandAlone/ResourceLimits.h>
#include <ThirdParty/glslang/SPIRV/GlslangToSpv.h>
#include <ThirdParty/SPIRV-Cross/spirv_hlsl.hpp>
#include <ThirdParty/dxc/inc/dxcapi.h>

struct ShaderCompileConfigs : public Serializeable<ShaderCompileConfigs>
{
	ShaderCompileConfigs(const char8_t* Path)
		: Serializeable(IAsset::CatPath(ASSET_PATH_SHADER_CACHE, Path))
	{
	}

	std::unordered_map<std::string, std::vector<std::vector<RHI::ShaderMacro>>> ShadersMacros;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(ShadersMacros)
		);
	}
};

class IShaderCompiler
{
public:
	virtual std::shared_ptr<RHI::ShaderDesc> Compile(
		const char8_t* SourceName,
		const void* Source,
		size_t Size,
		const char8_t* Entry,
		RHI::EShaderStage Stage,
		const std::vector<RHI::ShaderMacro>& Macros) = 0;

	static const char8_t* const ShaderModel(RHI::EShaderStage Stage, bool8_t DXC)
	{
		switch (Stage)
		{
		case RHI::EShaderStage::Vertex:   return DXC ? "vs_6_0" : "vs_5_0";
		case RHI::EShaderStage::Hull:     return DXC ? "hs_6_0" : "hs_5_0";
		case RHI::EShaderStage::Domain:   return DXC ? "ds_6_0" : "ds_5_0";
		case RHI::EShaderStage::Geometry: return DXC ? "gs_6_0" : "gs_5_0";
		case RHI::EShaderStage::Fragment: return DXC ? "ps_6_0" : "ps_5_0";
		case RHI::EShaderStage::Compute:  return DXC ? "cs_6_0" : "cs_5_0";
		}

		assert(0);
		return nullptr;
	}

	static std::shared_ptr<IShaderCompiler> Create(RHI::ERenderer Renderer);
protected:
	virtual void GetReflectionInfo(RHI::ShaderDesc* const Desc) = 0;
};

class DxcShaderCompiler : public IShaderCompiler
{
public:
	DxcShaderCompiler();

	std::shared_ptr<RHI::ShaderDesc> Compile(
		const char8_t* SourceName,
		const void* Source,
		size_t Size,
		const char8_t* Entry,
		RHI::EShaderStage Stage,
		const std::vector<RHI::ShaderMacro>& Macros) override
	{
		assert(Source && Entry && Stage < RHI::EShaderStage::ShaderStageCount);

		std::vector<const wchar_t*> Args;
		auto Desc = CompileWithArgs(
			SourceName,
			Source,
			Size,
			Entry,
			Stage,
			Macros,
			Args
		);

		GetReflectionInfo(Desc.get());

		return Desc;
	}
	std::shared_ptr<RHI::ShaderDesc> CompileWithArgs(
		const char8_t* SourceName,
		const void* Source,
		size_t Size,
		const char8_t* Entry,
		RHI::EShaderStage Stage,
		const std::vector<RHI::ShaderMacro>& Macros,
		std::vector<const wchar_t*>& Args);
protected:
	class DxcCompiler final : public RHI::D3DHWObject<void, IDxcCompiler3> {};
	class DxcBlobEncoding final : public RHI::D3DHWObject<void, IDxcBlobEncoding> {};
	class DxcResult final : public RHI::D3DHWObject<void, IDxcResult> {};
	class DxcBlob final : public RHI::D3DHWObject<void, IDxcBlob> {};
	class DxcUtils final : public RHI::D3DHWObject<void, IDxcUtils> {};
	class DxcCompilerArgs final : public RHI::D3DHWObject<void, IDxcCompilerArgs> {};
	class DxcIncludeHandler final : public RHI::D3DHWObject<void, IDxcIncludeHandler> {};
	class DxcContainerReflection final : public RHI::D3DHWObject<void, IDxcContainerReflection> {};
	class D3D12LibraryReflection final : public RHI::D3DHWObject<void, ID3D12LibraryReflection> {};
	class D3D12ShaderReflection final : public RHI::D3DHWObject<void, ID3D12ShaderReflection> {};

	void GetReflectionInfo(RHI::ShaderDesc* const Desc) override;
private:
	DxcUtils m_Utils;
	DxcCompiler m_Compiler;
	DxcBlobEncoding m_Blob;
};

class D3DShaderCompiler : public IShaderCompiler
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
	class D3D11ShaderReflection final : public RHI::D3DHWObject<void, ID3D11ShaderReflection> {};
	void GetReflectionInfo(RHI::ShaderDesc* const Desc) override final;
};

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