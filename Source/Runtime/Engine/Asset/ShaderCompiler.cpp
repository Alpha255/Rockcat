#include "Runtime/Engine/Asset/ShaderCompiler.h"
#include "Runtime/Engine/Asset/ShaderAsset.h"
#include <glslang/Include/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <spirv_cross/spirv_hlsl.hpp>

#if !defined(DXIL_FOURCC)
#define DXIL_FOURCC(ch0, ch1, ch2, ch3) (                                  \
		(uint32_t)(uint8_t)(ch0)        | (uint32_t)(uint8_t)(ch1) << 8  | \
		(uint32_t)(uint8_t)(ch2) << 16  | (uint32_t)(uint8_t)(ch3) << 24)
#endif

DxcShaderCompiler::DxcShaderCompiler(bool8_t GenerateSpirv)
	: m_GenSpirv(GenerateSpirv)
{
	VERIFY(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(m_Utils.Reference())) == S_OK);
	VERIFY(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(m_Compiler.Reference())) == S_OK);
}

std::shared_ptr<RHIShaderCreateInfo> DxcShaderCompiler::Compile(
	const char8_t* SourceName,
	const char8_t* SourceCode,
	size_t SourceCodeSize,
	const char8_t* ShaderEntryName,
	ERHIShaderStage ShaderStage,
	const ShaderDefines& Definitions)
{
	assert(SourceCode && SourceCodeSize && ShaderEntryName && ShaderStage < ERHIShaderStage::Num);

	VERIFY(m_Utils->CreateBlobFromPinned(SourceCode, static_cast<uint32_t>(SourceCodeSize), DXC_CP_ACP, m_Blob.Reference()) == S_OK);

	struct LocalShaderDefinition
	{
		std::wstring Name;
		std::wstring Value;
	};

	std::vector<LocalShaderDefinition> LocalShaderDefinitions(Definitions.GetDefines().size());
	std::vector<DxcDefine> DxcDefines(m_GenSpirv ? Definitions.GetDefines().size() + 1u : Definitions.GetDefines().size());

	uint32_t Index = 0u;
	for (const auto& [Name, Value] : Definitions.GetDefines())
	{
		LocalShaderDefinitions[Index].Name = std::move(StringUtils::ToWide(Name.c_str()));
		LocalShaderDefinitions[Index].Value = std::move(StringUtils::ToWide(Value.c_str()));

		DxcDefines[Index].Name = LocalShaderDefinitions.back().Name.c_str();
		DxcDefines[Index].Value = LocalShaderDefinitions.back().Value.c_str();

		++Index;
	}

	if (m_GenSpirv)
	{
		DxcDefines[Index].Name = L"SPIRV";
		DxcDefines[Index].Value = L"1";
	}

	std::vector<const wchar_t*> OtherArgs;
	auto IncludeDirectoryArg = StringUtils::ToWide(StringUtils::Format("-I %s", PlatformMisc::GetCurrentWorkingDirectory().generic_string().c_str()));
	OtherArgs.push_back(IncludeDirectoryArg.c_str());

	DxcCompilerArgs CompilerArgs;
	VERIFY(m_Utils->BuildArguments(
		SourceName ? StringUtils::ToWide(SourceName).c_str() : nullptr,
		StringUtils::ToWide(ShaderEntryName).c_str(),
		StringUtils::ToWide(GetShaderModelName(ShaderStage, true)).c_str(),
		OtherArgs.data(),
		static_cast<uint32_t>(OtherArgs.size()),
		DxcDefines.data(),
		static_cast<uint32_t>(DxcDefines.size()),
		CompilerArgs.Reference()) == S_OK);

	DxcBuffer Buffer
	{
		m_Blob->GetBufferPointer(),
		m_Blob->GetBufferSize(),
		DXC_CP_ACP
	};

	DxcIncludeHandler IncludeHandler;
	VERIFY(m_Utils->CreateDefaultIncludeHandler(IncludeHandler.Reference()) == S_OK);

	DxcResult Result;
	VERIFY(m_Compiler->Compile(
		&Buffer,
		CompilerArgs->GetArguments(),
		CompilerArgs->GetCount(),
		IncludeHandler.Get(),
		IID_PPV_ARGS(Result.Reference())) == S_OK);

	::HRESULT HR = E_FAIL;
	VERIFY(Result->GetStatus(&HR) == S_OK);

	if (FAILED(HR))
	{
		DxcBlobEncoding Error;
		VERIFY(Result->GetErrorBuffer(Error.Reference()) == S_OK);
		LOG_ERROR("ShaderCompiler:: {}", static_cast<const char8_t*>(Error->GetBufferPointer()));
		assert(0);
	}

	DxcBlob Blob;
	VERIFY(Result->GetResult(Blob.Reference()) == S_OK);

	auto ShaderCreateInfo = std::make_shared<RHIShaderCreateInfo>();
	ShaderCreateInfo->ShaderStage = ShaderStage;
	ShaderCreateInfo->Language = ERHIShaderLanguage::HLSL;
	//ShaderCreateInfo->BinarySize = Blob->GetBufferSize();
	ShaderCreateInfo->Name = SourceName ? std::string(SourceName) : "";

	//ShaderCreateInfo->Binary.resize(Align(Blob->GetBufferSize(), sizeof(uint32_t)) / sizeof(uint32_t));
	//VERIFY(memcpy_s(ShaderCreateInfo->Binary.data(), Blob->GetBufferSize(), Blob->GetBufferPointer(), Blob->GetBufferSize()) == 0);

	return ShaderCreateInfo;
}

std::shared_ptr<RHIShaderCreateInfo> D3DShaderCompiler::Compile(
	const char8_t* SourceName,
	const char8_t* SourceCode,
	size_t SourceCodeSize,
	const char8_t* ShaderEntryName,
	ERHIShaderStage ShaderStage,
	const class ShaderDefines& Definitions)
{
	assert(SourceCode && SourceCodeSize && ShaderEntryName && ShaderStage < ERHIShaderStage::Num);

	D3DBlob Binary;
	D3DBlob Error;

	// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/d3dcompile-constants
	// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/d3dcompile-effect-constants
#if defined(DEBUG) || defined(_DEBUG)
	uint32_t Flags = D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY | D3DCOMPILE_DEBUG;
#else
	uint32_t Flags = D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY; /// Forces strict compile, which might not allow for legacy syntax.
#endif

	uint32_t Index = 0u;
	std::vector<D3D_SHADER_MACRO> D3DMacros(Definitions.GetDefines().size());
	for each (const auto& NameValue in Definitions.GetDefines())
	{
		D3DMacros[Index].Name = NameValue.first.c_str();
		D3DMacros[Index].Definition = NameValue.second.c_str();
	}

	if (FAILED(::D3DCompile2(
		SourceCode,
		SourceCodeSize,
		SourceName,
		D3DMacros.data(),
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		ShaderEntryName,
		GetShaderModelName(ShaderStage, false),
		Flags,
		0u,
		0u,
		nullptr,
		0u,
		Binary.Reference(),
		Error.Reference())))
	{
		LOG_ERROR("D3DShaderCompiler:: Failed to compile shader: {}", reinterpret_cast<const char8_t* const>(Error->GetBufferPointer()));
		assert(0);
	}

	auto ShaderCreateInfo = std::shared_ptr<RHIShaderCreateInfo>();
	ShaderCreateInfo->ShaderStage = ShaderStage;
	ShaderCreateInfo->Language = ERHIShaderLanguage::HLSL;
	ShaderCreateInfo->Name = SourceName ? std::string(SourceName) : "";
	//ShaderCreateInfo->BinarySize = Binary->GetBufferSize();
	//ShaderCreateInfo->Binary.resize(Align(Binary->GetBufferSize(), sizeof(uint32_t)) / sizeof(uint32_t));
	//VERIFY(memcpy_s(Desc->Binary.data(), Binary->GetBufferSize(), Binary->GetBufferPointer(), Binary->GetBufferSize()) == 0);

	//GetReflectionInfo(Desc.get());

	return ShaderCreateInfo;
}

#if 0

#define CMD_SPRIV L"-spirv"

void DxcShaderCompiler::GetReflectionInfo(RHI::ShaderDesc* const Desc)
{
	DxcContainerReflection ContainerReflection;
	VERIFY(DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(ContainerReflection.Reference())) == S_OK);

	uint32_t Index = ~0u;
	VERIFY(ContainerReflection->Load(m_Blob.Get()) == S_OK);
	VERIFY(ContainerReflection->FindFirstPartKind(DXIL_FOURCC('D', 'X', 'I', 'L'), &Index) == S_OK);

	D3D12ShaderReflection D3DReflection;
	VERIFY(ContainerReflection->GetPartReflection(Index, IID_PPV_ARGS(D3DReflection.Reference())) == S_OK);

	D3D12_SHADER_DESC ShaderDesc;
	VERIFY(D3DReflection->GetDesc(&ShaderDesc) == S_OK);

	for (uint32_t CBIndex = 0u; CBIndex < ShaderDesc.ConstantBuffers; ++CBIndex)
	{
		D3D12_SHADER_BUFFER_DESC BufferDesc;
		VERIFY(D3DReflection->GetConstantBufferByIndex(CBIndex)->GetDesc(&BufferDesc) == S_OK);

		if (BufferDesc.Type == D3D_CT_CBUFFER) /// D3D_CT_TBUFFER: A buffer containing texture data
		{
			D3D12_SHADER_INPUT_BIND_DESC BindDesc;
			VERIFY(D3DReflection->GetResourceBindingDesc(CBIndex, &BindDesc) == S_OK);

			Desc->Variables.emplace_back(
				RHI::ShaderVariable::Description
				{
					RHI::EResourceType::UniformBuffer,
					BindDesc.BindPoint,
					Desc->Stage,
					BufferDesc.Size,
					BufferDesc.Name
				});
		}
	}
	for (uint32_t ResIndex = 0u; ResIndex < ShaderDesc.BoundResources; ++ResIndex)
	{
		D3D12_SHADER_INPUT_BIND_DESC BindDesc;
		VERIFY(D3DReflection->GetResourceBindingDesc(ResIndex, &BindDesc) == S_OK);

		switch (BindDesc.Type)
		{
		case D3D_SIT_CBUFFER:
		case D3D_SIT_TBUFFER:
			break;
		case D3D_SIT_TEXTURE:
			Desc->Variables.emplace_back(
				RHI::ShaderVariable::Description
				{
					RHI::EResourceType::SampledImage,
					BindDesc.BindPoint,
					Desc->Stage,
					~0u,
					BindDesc.Name
				});
			break;
		case D3D_SIT_SAMPLER:
			Desc->Variables.emplace_back(
				RHI::ShaderVariable::Description
				{
					RHI::EResourceType::Sampler,
					BindDesc.BindPoint,
					Desc->Stage,
					~0u,
					BindDesc.Name
				});
			break;
		case D3D_SIT_UAV_RWTYPED:
			break;
		case D3D_SIT_STRUCTURED:
			break;
		case D3D_SIT_UAV_RWSTRUCTURED:
			break;
		case D3D_SIT_BYTEADDRESS:
			break;
		case D3D_SIT_UAV_RWBYTEADDRESS:
			break;
		case D3D_SIT_UAV_APPEND_STRUCTURED:
			break;
		case D3D_SIT_UAV_CONSUME_STRUCTURED:
			break;
		case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			break;
		case D3D_SIT_RTACCELERATIONSTRUCTURE:
			break;
		case D3D_SIT_UAV_FEEDBACKTEXTURE:
			break;
		}
	}
}

void D3DShaderCompiler::GetReflectionInfo(RHI::ShaderDesc* const Desc)
{
	assert(Desc->BinarySize);

	D3D11ShaderReflection D3DReflection;
	VERIFY(::D3DReflect(Desc->Binary.data(), Desc->BinarySize, IID_PPV_ARGS(D3DReflection.Reference())) == S_OK);

	D3D11_SHADER_DESC ShaderDesc;
	VERIFY(D3DReflection->GetDesc(&ShaderDesc) == S_OK);

	for (uint32_t CBIndex = 0u; CBIndex < ShaderDesc.ConstantBuffers; ++CBIndex)
	{
		D3D11_SHADER_BUFFER_DESC BufferDesc;
		VERIFY(D3DReflection->GetConstantBufferByIndex(CBIndex)->GetDesc(&BufferDesc) == S_OK);

		if (BufferDesc.Type == D3D_CT_CBUFFER) /// D3D_CT_TBUFFER: A buffer containing texture data
		{
			D3D11_SHADER_INPUT_BIND_DESC BindDesc;
			VERIFY(D3DReflection->GetResourceBindingDesc(CBIndex, &BindDesc) == S_OK);

			Desc->Variables.emplace_back(
				RHI::ShaderVariable::Description
				{
					RHI::EResourceType::UniformBuffer,
					BindDesc.BindPoint,
					Desc->Stage,
					BufferDesc.Size,
					BufferDesc.Name
				});
		}
	}
	for (uint32_t ResIndex = 0u; ResIndex < ShaderDesc.BoundResources; ++ResIndex)
	{
		D3D11_SHADER_INPUT_BIND_DESC BindDesc;
		VERIFY(D3DReflection->GetResourceBindingDesc(ResIndex, &BindDesc) == S_OK);

		switch (BindDesc.Type)
		{
		case D3D_SIT_CBUFFER:
		case D3D_SIT_TBUFFER:
			break;
		case D3D_SIT_TEXTURE:
			Desc->Variables.emplace_back(
				RHI::ShaderVariable::Description
				{
					RHI::EResourceType::SampledImage,
					BindDesc.BindPoint,
					Desc->Stage,
					~0u,
					BindDesc.Name
				});
			break;
		case D3D_SIT_SAMPLER:
			Desc->Variables.emplace_back(
				RHI::ShaderVariable::Description
				{
					RHI::EResourceType::Sampler,
					BindDesc.BindPoint,
					Desc->Stage,
					~0u,
					BindDesc.Name
				});
			break;
		case D3D_SIT_UAV_RWTYPED:
			break;
		case D3D_SIT_STRUCTURED:
			break;
		case D3D_SIT_UAV_RWSTRUCTURED:
			break;
		case D3D_SIT_BYTEADDRESS:
			break;
		case D3D_SIT_UAV_RWBYTEADDRESS:
			break;
		case D3D_SIT_UAV_APPEND_STRUCTURED:
			break;
		case D3D_SIT_UAV_CONSUME_STRUCTURED:
			break;
		case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			break;
		case D3D_SIT_RTACCELERATIONSTRUCTURE:
			break;
		case D3D_SIT_UAV_FEEDBACKTEXTURE:
			break;
		}
	}
}

std::shared_ptr<RHI::ShaderDesc> VkShaderCompiler::Compile(
	const char8_t* SourceName,
	const void* Source,
	size_t Size,
	const char8_t* Entry,
	RHI::EShaderStage Stage,
	const std::vector<RHI::ShaderMacro>& Macros)
{
	assert(Source && Entry && Stage < RHI::EShaderStage::ShaderStageCount);
#if 1
	std::vector<const wchar_t*> Args
	{
		CMD_SPRIV
	};
	auto Desc = CompileWithArgs(
		SourceName,
		Source,
		Size,
		Entry,
		Stage,
		Macros,
		Args
	);
#else
	std::string preamble;
	for (auto& macro : macros)
	{
		preamble += StringUtils::format("%s %s\n", macro.Name.c_str(), macro.Definition.c_str());
	}

	EShMessages messages = static_cast<EShMessages>(EShMsgDefault | EShMsgVulkanRules | EShMsgSpvRules);
	const char8_t* const sources[] = { Source };
	glslang::TShader::ForbidIncluder noneIncluder;

	glslang::InitializeProcess();

	glslang::TShader shader(ShLang(stage));
	shader.setStrings(sources, _countof(sources));
	shader.setEntryPoint(entry);
	shader.setPreamble(preamble.c_str());

	if (!shader.parse(&glslang::DefaultTBuiltInResource,
		EVersion::OGL_Desktop,
		EProfile::ENoProfile,
		false,
		false,
		messages,
		noneIncluder))
	{
		LOG_ERROR("VkShaderCompiler:: {}. {}", shader.getInfoLog(), shader.getInfoDebugLog());
		assert(0);
	}

	glslang::TProgram program;
	program.addShader(&shader);

	if (!program.link(messages))
	{
		LOG_ERROR("VkShaderCompiler:: {}. {}", program.getInfoLog(), program.getInfoDebugLog());
		assert(0);
	}

	glslang::TIntermediate* intermediate = program.getIntermediate(shader.getStage()); /// buildReflection
	assert(intermediate);

	spv::SpvBuildLogger logger;
	glslang::SpvOptions options;

	auto desc = std::make_shared<ShaderDesc>();
	desc->Stage = stage;
	desc->Language = EShaderLanguage::GLSL;

	glslang::GlslangToSpv(*intermediate, desc->Binary, &logger, &options);
	if (desc->Binary.size() == 0)
	{
		LOG_ERROR("VkShaderCompiler:: {}", logger.getAllMessages());
		assert(0);
	}
	desc->BinarySize = desc->Binary.size() * sizeof(uint32_t);

	glslang::FinalizeProcess();
#endif

	GetReflectionInfo(Desc.get());

	return Desc;
}

void VkShaderCompiler::GetReflectionInfo(RHI::ShaderDesc* const Desc)
{
	assert(Desc->BinarySize);

	spirv_cross::Compiler Compiler(Desc->Binary);
	auto ActiveVars = Compiler.get_active_interface_variables();
	spirv_cross::ShaderResources ShaderResources = Compiler.get_shader_resources(ActiveVars);
	Compiler.set_enabled_interface_variables(std::move(ActiveVars));

	for (auto& PushConstants : ShaderResources.push_constant_buffers)
	{
		auto Type = Compiler.get_type(PushConstants.type_id);
		auto Size = static_cast<uint32_t>(Compiler.get_declared_struct_size(Type));

		Desc->Variables.emplace_back(
			RHI::ShaderVariable::Description
			{
				RHI::EResourceType::PushConstants,
				std::numeric_limits<uint8_t>().max(),
				Desc->Stage,
				Size,
				PushConstants.name
			});
	}
	for (auto& UniformBuffer : ShaderResources.uniform_buffers)
	{
		auto Type = Compiler.get_type(UniformBuffer.type_id);
		auto Binding = Compiler.get_decoration(UniformBuffer.id, spv::Decoration::DecorationBinding);
		auto Size = static_cast<uint32_t>(Compiler.get_declared_struct_size(Type));

		Desc->Variables.emplace_back(
			RHI::ShaderVariable::Description
			{
				RHI::EResourceType::UniformBuffer,
				Binding,
				Desc->Stage,
				Size,
				UniformBuffer.name
			});
	}
	for (auto& SampledImage : ShaderResources.sampled_images)
	{
		auto Binding = Compiler.get_decoration(SampledImage.id, spv::Decoration::DecorationBinding);
		Desc->Variables.emplace_back(
			RHI::ShaderVariable::Description
			{
				RHI::EResourceType::CombinedImageSampler,
				Binding,
				Desc->Stage,
				~0u,
				SampledImage.name
			});
	}
	for (auto& Image : ShaderResources.separate_images)
	{
		auto Binding = Compiler.get_decoration(Image.id, spv::Decoration::DecorationBinding);
		Desc->Variables.emplace_back(
			RHI::ShaderVariable::Description
			{
				RHI::EResourceType::SampledImage,
				Binding,
				Desc->Stage,
				~0u,
				Image.name
			});
	}
	for (auto& Sampler : ShaderResources.separate_samplers)
	{
		auto Binding = Compiler.get_decoration(Sampler.id, spv::Decoration::DecorationBinding);
		Desc->Variables.emplace_back(
			RHI::ShaderVariable::Description
			{
				RHI::EResourceType::Sampler,
				Binding,
				Desc->Stage,
				~0u,
				Sampler.name
			});
	}
}
#endif

std::shared_ptr<IShaderCompiler> IShaderCompiler::Create(ERenderHardwareInterface RHI)
{
	assert(RHI > ERenderHardwareInterface::Null);

	switch (RHI)
	{
	case ERenderHardwareInterface::D3D11:
		return std::make_shared<D3DShaderCompiler>();
	case ERenderHardwareInterface::D3D12:
		return std::make_shared<DxcShaderCompiler>(false);
	case ERenderHardwareInterface::Vulkan:
		return std::make_shared<DxcShaderCompiler>(true);
	case ERenderHardwareInterface::Software:
	default:
		return nullptr;
	}
}