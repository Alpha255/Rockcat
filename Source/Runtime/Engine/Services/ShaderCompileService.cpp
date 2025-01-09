#include "Engine/Services/ShaderCompileService.h"
#include "Engine/Async/Task.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/Paths.h"
#pragma warning(disable:4068)
#include <Submodules/filewatch/FileWatch.hpp>
#pragma warning(default:4068)

class ShaderCompileTask : public Task
{
public:
	ShaderCompileTask(const char* ShaderFilePath)
		: Task(std::move(StringUtils::Format("ShaderCompileTask|%s", ShaderFilePath)), EPriority::High)
	{
	}

	void Execute() override final
	{
	}
};

void ShaderCompileService::OnStartup()
{
	REGISTER_LOG_CATEGORY(LogShaderCompile);

	m_Compilers[(size_t)ERenderHardwareInterface::Vulkan] = std::make_unique<DxcShaderCompiler>(true);
	m_Compilers[(size_t)ERenderHardwareInterface::D3D11] = std::make_unique<D3DShaderCompiler>();
	m_Compilers[(size_t)ERenderHardwareInterface::D3D12] = std::make_unique<DxcShaderCompiler>(false);

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
}

void ShaderCompileService::OnShaderFileModified(const std::string& FilePath)
{
	LOG_INFO("{} is modified.", FilePath);
}

void ShaderCompileService::Compile(const ShaderAsset& Shader)
{
	const size_t Hash = Shader.ComputeHash();
	const auto FileName = Shader.GetPath().filename().generic_string();
	const auto SourceCode = reinterpret_cast<char*>(Shader.GetRawData().Data.get());
	const auto Size = Shader.GetRawData().Size;

	for (uint32_t Index = (uint32_t)ERenderHardwareInterface::Vulkan; Index < (uint32_t)ERenderHardwareInterface::Num; ++Index)
	{
		auto RHI = static_cast<ERenderHardwareInterface>(Index);

		if (RegisterCompileTask(RHI, Hash))
		{
			auto Binary = GetCompiler(RHI).Compile(
				FileName.c_str(),
				SourceCode,
				Size,
				"main",
				Shader.GetStage(),
				Shader);

			DeregisterCompileTask(RHI, Hash);
		}
	}
}

bool ShaderCompileService::RegisterCompileTask(ERenderHardwareInterface RHI, size_t Hash)
{
	assert(RHI < ERenderHardwareInterface::Num);

	if (m_CompilingTasks[size_t(RHI)].find(Hash) != m_CompilingTasks[size_t(RHI)].cend())
	{
		return false;
	}

	m_CompilingTasks[size_t(RHI)].insert(Hash);
	return true;
}

void ShaderCompileService::DeregisterCompileTask(ERenderHardwareInterface RHI, size_t Hash)
{
	assert(RHI < ERenderHardwareInterface::Num);
	m_CompilingTasks[size_t(RHI)].erase(Hash);
}

void ShaderCompileService::OnShutdown()
{
	for (uint32_t Index = 0u; Index < m_Compilers.size(); ++Index)
	{
		m_Compilers[Index].reset();
	}

	m_ShaderFileMonitor.reset();
}