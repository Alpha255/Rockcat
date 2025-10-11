#include "Asset/ShaderCompiler.h"

DxcShaderCompiler::DxcShaderCompiler(bool GenerateSpirv)
	: m_GenSpirv(GenerateSpirv)
{
	VERIFY(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(m_Utils.Reference())) == S_OK);
	VERIFY(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(m_Compiler.Reference())) == S_OK);

	DxcVersionInfo VersionInfo;
	VERIFY(m_Compiler->QueryInterface(IID_PPV_ARGS(VersionInfo.Reference())) == S_OK);

	uint32_t Major = 0u, Minor = 0u;
	VERIFY(VersionInfo->GetVersion(&Major, &Minor) == S_OK);

	LOG_CAT_INFO(LogShaderCompiler, "Create dxc shader compiler v{}.{}, spir-v is {}", Major, Minor, GenerateSpirv ? "enabled" : "disabled");
}

ShaderBlob DxcShaderCompiler::Compile(
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
		LOG_CAT_ERROR(LogShaderCompiler, "Failed to compile shader: {}, error message: {}", SourceName, static_cast<const char*>(Error->GetBufferPointer()));
		return ShaderBlob(0u);
	}

	DxcBlob Blob;
	VERIFY(Result->GetResult(Blob.Reference()) == S_OK);

	return ShaderBlob(Blob->GetBufferSize(), Blob->GetBufferPointer());
}

ShaderBlob D3DShaderCompiler::Compile(
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
		LOG_CAT_ERROR(LogShaderCompiler, "Failed to compile shader: {}, error message: {}", SourceName, reinterpret_cast<const char* const>(Error->GetBufferPointer()));
		assert(0);
	}

	return ShaderBlob(Binary->GetBufferSize(), Binary->GetBufferPointer());
}
