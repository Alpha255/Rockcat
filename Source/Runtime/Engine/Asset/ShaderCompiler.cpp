#include "Engine/Asset/ShaderCompiler.h"
#include "Engine/Async/Task.h"

#if !defined(DXIL_FOURCC)
#define DXIL_FOURCC(ch0, ch1, ch2, ch3) (                                  \
		(uint32_t)(uint8_t)(ch0)        | (uint32_t)(uint8_t)(ch1) << 8  | \
		(uint32_t)(uint8_t)(ch2) << 16  | (uint32_t)(uint8_t)(ch3) << 24)
#endif

DxcShaderCompiler::DxcShaderCompiler(bool GenerateSpirv)
	: m_GenSpirv(GenerateSpirv)
{
	VERIFY(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(m_Utils.Reference())) == S_OK);
	VERIFY(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(m_Compiler.Reference())) == S_OK);

	DxcVersionInfo VersionInfo;
	VERIFY(m_Utils->QueryInterface(IID_PPV_ARGS(VersionInfo.Reference())) == S_OK);

	uint32_t Major = 0u, Minor = 0u;
	VERIFY(VersionInfo->GetVersion(&Major, &Minor) == S_OK);

	LOG_INFO("DxcShaderCompiler:: Create dxc shader compiler @{}.{}, generate spir-v is {}", Major, Minor, GenerateSpirv ? "enabled" : "disabled");
}

ShaderBinary DxcShaderCompiler::Compile(
	const char* SourceName,
	const char* SourceCode,
	size_t SourceCodeSize,
	const char* EntryPoint,
	ERHIShaderStage ShaderStage,
	const ShaderDefines& Definitions)
{
	assert(SourceCode && SourceCodeSize && EntryPoint);

	DxcBlobEncoding SourceBlob;

	VERIFY(m_Utils->CreateBlobFromPinned(SourceCode, static_cast<uint32_t>(SourceCodeSize), DXC_CP_ACP, SourceBlob.Reference()) == S_OK);

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
		++Index;
	}

	Index = 0u;
	for (const auto& Define : LocalShaderDefinitions)
	{
		DxcDefines[Index].Name = Define.Name.c_str();
		DxcDefines[Index].Value = Define.Value.c_str();
		++Index;
	}

	if (m_GenSpirv)
	{
		DxcDefines[Index].Name = L"_SPIRV_";
		DxcDefines[Index].Value = L"1";
	}

	auto WSourceName = SourceName ? StringUtils::ToWide(SourceName).c_str() : std::wstring();
	auto WEntryPoint = StringUtils::ToWide(EntryPoint);
	auto WProfile = StringUtils::ToWide(GetProfile(ShaderStage, true));

	std::vector<const wchar_t*> Args;
	//auto IncludeDirectoryArg = StringUtils::ToWide(StringUtils::Format("-I %s", ASSET_PATH_SHADERS));
	//Args.push_back(IncludeDirectoryArg.c_str());
	if (m_GenSpirv)
	{
		Args.push_back(L"-spirv");
		//Args.push_back(L"-fspv-debug=vulkan-with-source");
	}
#if _DEBUG
	Args.push_back(L"-Zi");
	Args.push_back(L"-Qembed_debug");
#endif

	DxcCompilerArgs CompilerArgs;
	VERIFY(m_Utils->BuildArguments(
		WSourceName.c_str(),
		WEntryPoint.c_str(),
		WProfile.c_str(),
		Args.data(),
		static_cast<uint32_t>(Args.size()),
		DxcDefines.data(),
		static_cast<uint32_t>(DxcDefines.size()),
		CompilerArgs.Reference()) == S_OK);

	DxcBuffer Buffer
	{
		SourceBlob->GetBufferPointer(),
		SourceBlob->GetBufferSize(),
		DXC_CP_ACP
	};

	DxcIncludeHandler IncludeHandler;
	VERIFY(m_Utils->CreateDefaultIncludeHandler(IncludeHandler.Reference()) == S_OK);

	DxcResult Result;
	VERIFY(m_Compiler->Compile(
		&Buffer,
		CompilerArgs->GetArguments(),
		CompilerArgs->GetCount(),
		IncludeHandler.GetNative(),
		IID_PPV_ARGS(Result.Reference())) == S_OK);

	::HRESULT HR = E_FAIL;
	VERIFY(Result->GetStatus(&HR) == S_OK);

	if (FAILED(HR))
	{
		DxcBlobEncoding Error;
		VERIFY(Result->GetErrorBuffer(Error.Reference()) == S_OK);
		LOG_ERROR("ShaderCompiler:: {}", static_cast<const char*>(Error->GetBufferPointer()));
		assert(0);
	}

	DxcBlob Blob;
	VERIFY(Result->GetResult(Blob.Reference()) == S_OK);

	return ShaderBinary(Blob->GetBufferSize(), Blob->GetBufferPointer());
}

ShaderBinary D3DShaderCompiler::Compile(
	const char* SourceName,
	const char* SourceCode,
	size_t SourceCodeSize,
	const char* EntryPoint,
	ERHIShaderStage ShaderStage,
	const ShaderDefines& Definitions)
{
	assert(SourceCode && SourceCodeSize && EntryPoint);

	D3DBlob Binary;
	D3DBlob Error;

	// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/d3dcompile-constants
	// https://learn.microsoft.com/en-us/windows/win32/direct3dhlsl/d3dcompile-effect-constants
#if defined(DEBUG) || defined(_DEBUG)
	uint32_t Flags = D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY | D3DCOMPILE_DEBUG;
#else
	uint32_t Flags = D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY; /// Forces strict compile, which might not allow for legacy syntax.
#endif

	// The last structure in the array serves as a terminator and must have all members set to NULL.
	uint32_t Index = 0u;
	std::vector<D3D_SHADER_MACRO> D3DMacros(Definitions.GetDefines().size() + 1u);
	for (const auto& [Name, Value] : Definitions.GetDefines())
	{
		D3DMacros[Index].Name = Name.c_str();
		D3DMacros[Index].Definition = Value.c_str();
		++Index;
	}

	// This default include handler includes files that are relative to the current directory and files that are relative to the directory of the initial source file. 
	// When you use D3D_COMPILE_STANDARD_FILE_INCLUDE, you must specify the source file name in the pSourceName parameter; 
	// the compiler will derive the initial relative directory from pSourceName.
	if (FAILED(::D3DCompile2(
		SourceCode,
		SourceCodeSize,
		SourceName,
		D3DMacros.data(),
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		EntryPoint,
		GetProfile(ShaderStage, false),
		Flags,
		0u,
		0u,
		nullptr,
		0u,
		Binary.Reference(),
		Error.Reference())))
	{
		LOG_ERROR("D3DShaderCompiler:: Failed to compile shader: {}", reinterpret_cast<const char* const>(Error->GetBufferPointer()));
		assert(0);
	}

	return ShaderBinary(Binary->GetBufferSize(), Binary->GetBufferPointer());
}

#if 0
#include <glslang/Include/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <spirv_cross/spirv_hlsl.hpp>

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
	const char* SourceName,
	const void* Source,
	size_t Size,
	const char* Entry,
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
	const char* const sources[] = { Source };
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
				std::numeric_limits<uint8_t>::max(),
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
