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

void ShaderLibrary::Initialize()
{
	REGISTER_LOG_CATEGORY(LogShaderLibrary);

	m_Compilers[ERHIDeviceType::Vulkan] = std::make_unique<DxcShaderCompiler>(true);
	m_Compilers[ERHIDeviceType::D3D11] = std::make_unique<D3DShaderCompiler>();
	m_Compilers[ERHIDeviceType::D3D12] = std::make_unique<DxcShaderCompiler>(false);

#if SHADER_HOT_RELOAD
	auto ShaderPath = Paths::ShaderPath().string();
	m_ShaderFileWatch = std::make_shared<filewatch::FileWatch<std::string>>(ShaderPath, std::regex(R"(.*\.(vert|frag|comp|hlsl|hlsli)$)"),
		[this](const std::string& SubPath, filewatch::Event FileEvent) {
			static std::unordered_map<std::string, std::chrono::steady_clock::time_point> s_EventTimeMap;
			static const std::chrono::milliseconds s_DeltaTime(500);

			switch (FileEvent)
			{
			case filewatch::Event::modified:
			{
				auto Now = std::chrono::steady_clock::now();
				auto& LastTimepoint = s_EventTimeMap[SubPath];
				if (Now - LastTimepoint < s_DeltaTime)
				{
					return;
				}

				LastTimepoint = Now;
				OnShaderFileModified(Paths::ShaderPath() / SubPath);
				break;
			}
			}
		});
#endif
}

void ShaderLibrary::OnShaderFileModified(const std::filesystem::path& Path)
{
	std::lock_guard Lock(m_ShaderLock);

	auto HeaderIt = m_HeaderFileInfos.find(Path);
	if (HeaderIt == m_HeaderFileInfos.cend())
	{
		auto ShaderIt = m_Shaders.find(Path);
		if (ShaderIt != m_Shaders.cend())
		{
			for (auto DeviceType : m_ActiveCompilers)
			{
				QueueCompileShader(*ShaderIt->second, DeviceType);
			}
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
					for (auto DeviceType : m_ActiveCompilers)
					{
						QueueCompileShader(*LinkedShader, DeviceType);
					}
				}
				else
				{
					HeaderIt->second.erase(LinkedShader);
				}
			}
		}
	}
}

void ShaderLibrary::RegisterShader(Shader& InShader)
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

void ShaderLibrary::CompileShader(Shader& InShader, ERHIDeviceType DeviceType)
{
	assert(std::filesystem::exists(InShader.GetPath()));

	RegisterShader(InShader);

	auto CachedBinaryPath = ShaderBinary::GetPath(InShader, DeviceType);
	if (std::filesystem::exists(CachedBinaryPath))
	{
		std::shared_ptr<ShaderBinary> CachedBinary = ShaderBinary::Load(CachedBinaryPath);
		if (CachedBinary->GetBlob().IsValid() && CachedBinary->m_ShaderLastWriteTime == InShader.GetLastWriteTime())
		{
			InShader.SetBinary(CachedBinary);
			LOG_CAT_DEBUG(LogShaderLibrary, "Load shader binary from \"{}\".", CachedBinaryPath.string());
			return;
		}
	}

	size_t Hash = ComputeHash(std::hash<Shader>()(InShader), DeviceType);

	if (RegisterCompileTask(Hash))
	{
		if (auto Compiler = GetCompiler(DeviceType))
		{
			std::ifstream FileStream(InShader.GetPath(), std::ios::in);
			assert(FileStream.is_open());
			std::string SourceCode(Asset::GetSize(InShader.GetPath()), '\0');
			FileStream.read(SourceCode.data(), SourceCode.size());
			FileStream.close();
			auto Blob = Compiler->Compile(
				InShader.GetStem().c_str(), 
				SourceCode.data(), 
				SourceCode.size(),
				InShader.GetEntryPoint(),
				InShader.GetStage(),
				InShader);
			if (Blob.IsValid())
			{
				InShader.SetBlob(Blob, DeviceType);
				LOG_CAT_INFO(LogShaderLibrary, "Shader \"{}\" compile success for device \"{}\".", InShader.GetStem(), magic_enum::enum_name(DeviceType).data());
			}
		}
		else
		{
			LOG_CAT_ERROR(LogShaderLibrary, "Unsupported device type \"{}\" for shader \"{}\"", magic_enum::enum_name(DeviceType).data(), InShader.GetStem());
		}

		DeregisterCompileTask(Hash);
	}
	else
	{
		LOG_CAT_DEBUG(LogShaderLibrary, "The shader \"{}\" is already in compiling queue", InShader.GetStem());
	}
}

void ShaderLibrary::QueueCompileShader(Shader& InShader, ERHIDeviceType DeviceType)
{
	tf::Async([this, &InShader, DeviceType]() {
		CompileShader(InShader, DeviceType);
	}, EThread::WorkerThread, Task::EPriority::High);
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

void ShaderLibrary::Finalize()
{
	m_HeaderFileInfos.clear();
	m_Shaders.clear();

	for (auto& Compiler : m_Compilers)
	{
		if (Compiler)
		{
			Compiler.reset();
		}
	}
	m_ShaderFileWatch.reset();
}