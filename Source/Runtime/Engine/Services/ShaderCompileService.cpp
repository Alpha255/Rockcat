#include "Engine/Services/ShaderCompileService.h"
#include "Engine/Async/Task.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/Paths.h"
#pragma warning(disable:4068)
#include <Submodules/filewatch/FileWatch.hpp>
#pragma warning(default:4068)

struct ShaderCompileTask : public Task
{
	ShaderCompileTask(ShaderAsset& InShader, IShaderCompiler& InCompiler)
		: Shader(InShader)
		, Compiler(InCompiler)
		, Task(std::move(StringUtils::Format("ShaderCompileTask|%s", InShader.GetPath().string().c_str())), EPriority::High)
	{
	}

	void Execute() override final
	{
		Shader.ReadRawData(AssetType::EContentsType::Text);

		const size_t Hash = Shader.ComputeHash();
		const auto FileName = Shader.GetPath().filename().string();
		const auto SourceCode = reinterpret_cast<char*>(Shader.GetRawData().Data.get());
		const auto Size = Shader.GetRawData().Size;
	}

	ShaderAsset& Shader;
	IShaderCompiler& Compiler;
};

void ShaderCompileService::OnStartup()
{
	REGISTER_LOG_CATEGORY(LogShaderCompile);

	m_Compilers[ERHIBackend::Vulkan] = std::make_unique<DxcShaderCompiler>(true);
	m_Compilers[ERHIBackend::D3D11] = std::make_unique<D3DShaderCompiler>();
	m_Compilers[ERHIBackend::D3D12] = std::make_unique<DxcShaderCompiler>(false);

#if SHADER_HOT_RELOAD
	auto ShaderPath = Paths::ShaderPath().string();
	m_ShaderFileMonitor = std::make_shared<filewatch::FileWatch<std::string>>(ShaderPath/*, std::regex("[*.vert, *.frag, *.comp, *.geom]")*/,
		[this](const std::string& Path, filewatch::Event FileEvent) {
			switch (FileEvent)
			{
			case filewatch::Event::modified:
				OnShaderFileModified(Path);
				break;
			}
		});
#endif
}

void ShaderCompileService::OnShaderFileModified(const std::string& FilePath)
{
	LOG_INFO("{} is modified.", FilePath);
}

void ShaderCompileService::Compile(ShaderAsset& Shader, ERHIBackend RHI)
{
	//for (uint32_t Index = (uint32_t)ERHIBackend::Vulkan; Index < (uint32_t)ERHIBackend::Num; ++Index)
	//{
	//	auto RHI = static_cast<ERHIBackend>(Index);

	//	if (RegisterCompileTask(RHI, Hash))
	//	{
	//		auto Binary = GetCompiler(RHI).Compile(
	//			FileName.c_str(),
	//			SourceCode,
	//			Size,
	//			"main",
	//			Shader.GetStage(),
	//			Shader);

	//		DeregisterCompileTask(RHI, Hash);
	//	}
	//}
}

bool ShaderCompileService::RegisterCompileTask(ERHIBackend RHI, size_t Hash)
{
	assert(RHI < ERHIBackend::Num);

	if (m_CompilingTasks[RHI].find(Hash) != m_CompilingTasks[RHI].cend())
	{
		return false;
	}

	m_CompilingTasks[RHI].insert(Hash);
	return true;
}

void ShaderCompileService::DeregisterCompileTask(ERHIBackend RHI, size_t Hash)
{
	assert(RHI < ERHIBackend::Num);
	m_CompilingTasks[RHI].erase(Hash);
}

void ShaderCompileService::OnShutdown()
{
	for (auto& Compiler : m_Compilers)
	{
		Compiler.reset();
	}

	m_ShaderFileMonitor.reset();
}