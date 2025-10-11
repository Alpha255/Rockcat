#include "Services/ShaderLibrary.h"
#include "Async/Task.h"
#include "Services/SpdLogService.h"
#include "Services/TaskFlowService.h"
#include "Paths.h"
#include "RHI/RHIPipeline.h"
#include "RHI/RHIDevice.h"
#pragma warning(disable:4068)
#include <filewatch/FileWatch.hpp>
#pragma warning(default:4068)

ShaderLibrary::ShaderLibrary(ERHIDeviceType DeviceType)
{
	REGISTER_LOG_CATEGORY(LogShaderCompiler);

	switch (DeviceType)
	{
	case ERHIDeviceType::Vulkan:
		m_Compiler = std::make_unique<DxcShaderCompiler>(true);
		break;
	case ERHIDeviceType::D3D11:
		m_Compiler = std::make_unique<D3DShaderCompiler>();
		break;
	case ERHIDeviceType::D3D12:
		m_Compiler = std::make_unique<DxcShaderCompiler>(false);
		break;
	}

#if SHADER_HOT_RELOAD
	auto ShaderPath = Paths::ShaderPath().string();
	m_ShaderFileWatch = std::make_shared<filewatch::FileWatch<std::string>>(ShaderPath, std::regex(R"(.*\.(vert|frag|comp|hlsl|hlsli)$)"),
		[this](const std::string& SubPath, filewatch::Event FileEvent) {
			switch (FileEvent)
			{
			case filewatch::Event::modified:
				OnShaderFileModified(Paths::ShaderPath() / SubPath);
				break;
			}
		});
#endif
}

void ShaderLibrary::OnShaderFileModified(const std::filesystem::path& Path)
{
	auto HeaderIt = m_HeaderFileInfos.find(Path);
	if (HeaderIt == m_HeaderFileInfos.cend())
	{
		auto ShaderIt = m_Shaders.find(Path);
		if (ShaderIt != m_Shaders.cend())
		{
			QueueCompileShader(*ShaderIt->second);
		}
	}
	else
	{
		// Modified file is a header file
		for (auto LinkedShader : HeaderIt->second)
		{
			if (LinkedShader)
			{
				auto IncludeFiles = ParseIncludeFiles(*LinkedShader);
				if (IncludeFiles.contains(Path))
				{
					QueueCompileShader(*LinkedShader);
				}
				else
				{
					HeaderIt->second.erase(LinkedShader);
				}
			}
		}
	}
}

void ShaderLibrary::RegisterShader(const Shader& InShader)
{
	std::lock_guard Lock(m_ShaderLock);
	m_Shaders[InShader.GetPath()] = &InShader;
	for (auto& HeaderFile : ParseIncludeFiles(InShader))
	{
		m_HeaderFileInfos[HeaderFile].insert(&InShader);
	}
}

std::unordered_set<std::filesystem::path> ShaderLibrary::ParseIncludeFiles(const Shader& InShader)
{
	std::unordered_set<std::filesystem::path> IncludeFiles;

	if (std::filesystem::exists(InShader.GetPath()))
	{
		std::ifstream ShaderFile(InShader.GetPath());
		if (ShaderFile.is_open())
		{
			std::string Line;
			std::regex IncludeRegex(R"(^\s*#\s*include\s*([<"])([^>"]+)[>"])");
			while (std::getline(ShaderFile, Line))
			{
				std::smatch Match;
				if (std::regex_search(Line, Match, IncludeRegex))
				{
					if (Match.size() == 3)
					{
						IncludeFiles.emplace((Paths::ShaderPath().parent_path() / Match[2].str()).make_preferred());
					}
				}
			}
			ShaderFile.close();
		}
	}

	return IncludeFiles;
}

void ShaderLibrary::CompileShader(const Shader& InShader)
{

}

void ShaderLibrary::QueueCompileShader(const Shader& InShader)
{

}

bool ShaderLibrary::RegisterCompileTask(size_t Hash)
{
	std::lock_guard Lock(m_CompileTaskLock);
	if (m_CompilingTasks.find(Hash) != m_CompilingTasks.cend())
	{
		return false;
	}

	m_CompilingTasks.insert(Hash);
	return true;
}

void ShaderLibrary::DeregisterCompileTask(size_t Hash)
{
	std::lock_guard Lock(m_CompileTaskLock);
	m_CompilingTasks.erase(Hash);
}

ShaderLibrary::~ShaderLibrary()
{
	m_Compiler.reset();
	m_ShaderFileWatch.reset();
}