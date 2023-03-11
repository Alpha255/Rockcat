#include "Runtime/Asset/ShaderCompiler.h"

#if !defined(DXIL_FOURCC)
#define DXIL_FOURCC(ch0, ch1, ch2, ch3) (                                  \
		(uint32_t)(uint8_t)(ch0)        | (uint32_t)(uint8_t)(ch1) << 8  | \
		(uint32_t)(uint8_t)(ch2) << 16  | (uint32_t)(uint8_t)(ch3) << 24)
#endif

#define CMD_SPRIV L"-spirv"

DxcShaderCompiler::DxcShaderCompiler()
{
	VERIFY(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(m_Utils.Reference())) == S_OK);
	VERIFY(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(m_Compiler.Reference())) == S_OK);
}

std::shared_ptr<RHI::ShaderDesc> DxcShaderCompiler::CompileWithArgs(
	const char8_t* SourceName,
	const void* Source,
	size_t Size,
	const char8_t* Entry,
	RHI::EShaderStage Stage,
	const std::vector<RHI::ShaderMacro>& Macros,
	std::vector<const wchar_t*>& Args)
{
	assert(Source && Entry && Stage < RHI::EShaderStage::ShaderStageCount);

	VERIFY(m_Utils->CreateBlobFromPinned(Source, static_cast<uint32_t>(Size), DXC_CP_ACP, m_Blob.Reference()) == S_OK);

	std::vector<const wchar_t*> CompileArgs(Args);

	const bool8_t IsSpirv = std::find_if(CompileArgs.begin(), CompileArgs.end(), [](auto Arg) {
		return lstrcmpW(Arg, CMD_SPRIV) == 0;
	}) != CompileArgs.end();

	struct ShaderDefine
	{
		std::wstring Name;
		std::wstring Value;
	};

	std::vector<ShaderDefine> Defines(Macros.size());
	std::vector<DxcDefine> DXCDefines(IsSpirv ? Macros.size() + 1u: Macros.size());

	for (uint32_t Index = 0u; Index < Macros.size(); ++Index)
	{
		Defines[Index] = ShaderDefine
		{
			String::ToWide(Macros[Index].Name),
			String::ToWide(Macros[Index].Definition)
		};
		DXCDefines[Index] = DxcDefine
		{
			Defines[Index].Name.c_str(),
			Defines[Index].Value.c_str()
		};
	}

	if (IsSpirv)
	{
		DXCDefines[DXCDefines.size() - 1ull]= DxcDefine
		{
			L"SPIRV",
			L"1"
		};
	}

	auto IncludeDirectoryArg = String::ToWide(String::Format("-I %s", Platform::GetCurrentWorkingDirectory().c_str()));
	CompileArgs.push_back(IncludeDirectoryArg.c_str());

	DxcCompilerArgs CompilerArgs;
	m_Utils->BuildArguments(
		String::ToWide(SourceName).c_str(),
		String::ToWide(Entry).c_str(),
		String::ToWide(ShaderModel(Stage, true)).c_str(),
		CompileArgs.data(),
		static_cast<uint32_t>(CompileArgs.size()),
		DXCDefines.data(),
		static_cast<uint32_t>(DXCDefines.size()),
		CompilerArgs.Reference());

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

	auto Desc = std::make_shared<RHI::ShaderDesc>();
	Desc->Stage = Stage;
	Desc->Language = RHI::EShaderLanguage::HLSL;
	Desc->BinarySize = Blob->GetBufferSize();
	Desc->Name = SourceName;

	Desc->Binary.resize(Align(Blob->GetBufferSize(), sizeof(uint32_t)) / sizeof(uint32_t));
	VERIFY(memcpy_s(Desc->Binary.data(), Blob->GetBufferSize(), Blob->GetBufferPointer(), Blob->GetBufferSize()) == 0);

	return Desc;
}

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

std::shared_ptr<RHI::ShaderDesc> D3DShaderCompiler::Compile(
	const char8_t* SourceName,
	const void* Source,
	size_t Size,
	const char8_t* Entry,
	RHI::EShaderStage Stage,
	const std::vector<RHI::ShaderMacro>& Macros)
{
	assert(Source && Entry && Stage < RHI::EShaderStage::ShaderStageCount);

	RHI::D3DBlob Binary;
	RHI::D3DBlob Error;

#if defined(DEBUG) || defined(_DEBUG)
	uint32_t Flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG;
#else
	uint32_t Flags = D3DCOMPILE_ENABLE_STRICTNESS; /// Forces strict compile, which might not allow for legacy syntax.
#endif

	std::vector<D3D_SHADER_MACRO> D3DMacros(Macros.size());
	for (uint32_t Index = 0u; Index < Macros.size(); ++Index)
	{
		D3DMacros[Index] = D3D_SHADER_MACRO
		{
			Macros[Index].Name.c_str(),
			Macros[Index].Definition.c_str()
		};
	}

	if (FAILED(::D3DCompile2(
		Source,
		Size,
		SourceName,
		D3DMacros.data(),
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		Entry,
		ShaderModel(Stage, false),
		Flags,
		0u,
		0u,
		nullptr,
		0u,
		Binary.Reference(),
		Error.Reference())))
	{
		LOG_ERROR("D3DShaderCompiler:: {}", reinterpret_cast<const char8_t* const>(Error->GetBufferPointer()));
		assert(0);
	}

	auto Desc = std::shared_ptr<RHI::ShaderDesc>();
	Desc->Stage = Stage;
	Desc->Language = RHI::EShaderLanguage::HLSL;
	Desc->BinarySize = Binary->GetBufferSize();
	Desc->Binary.resize(Align(Binary->GetBufferSize(), sizeof(uint32_t)) / sizeof(uint32_t));
	VERIFY(memcpy_s(Desc->Binary.data(), Binary->GetBufferSize(), Binary->GetBufferPointer(), Binary->GetBufferSize()) == 0);

	GetReflectionInfo(Desc.get());

	return Desc;
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
		preamble += String::format("%s %s\n", macro.Name.c_str(), macro.Definition.c_str());
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

std::shared_ptr<IShaderCompiler> IShaderCompiler::Create(RHI::ERenderer Eenderer)
{
	assert(Eenderer > RHI::ERenderer::Null);

	switch (Eenderer)
	{
	case RHI::ERenderer::D3D11:
		return std::make_shared<D3DShaderCompiler>();
	case RHI::ERenderer::D3D12:
		return std::make_shared<DxcShaderCompiler>();
	case RHI::ERenderer::Vulkan:
		return std::make_shared<VkShaderCompiler>();
	case RHI::ERenderer::Software:
		assert(0);
		break;
	}

	return nullptr;
}