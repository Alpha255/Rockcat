#include "Engine/Services/ShaderLibrary.h"
#include "Engine/Async/Task.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/Services/TaskFlowService.h"
#include "Engine/Paths.h"
#include "Engine/RHI/RHIPipeline.h"
#include "Engine/RHI/RHIBackend.h"
#pragma warning(disable:4068)
#include <Submodules/filewatch/FileWatch.hpp>
#pragma warning(default:4068)

const RHIShader* ShaderLibrary::ShaderPermutation::GetOrCreateRHI(RHIDevice& Device)
{
	if (!Module && Binary->IsValid())
	{
		RHIShaderCreateInfo ShaderCreateInfo;
		ShaderCreateInfo.SetStage(Binary->GetStage())
			.SetShaderBinary(Binary.get())
			.SetName(Binary->GetName().string());
		Module = Device.CreateShader(ShaderCreateInfo);
	}

	return Module.get();
}

ShaderLibrary::ShaderLibrary(ERHIBackend Backend, RHIDevice& Device)
	: m_Device(Device)
	, m_Backend(Backend)
{
	REGISTER_LOG_CATEGORY(LogShaderCompile);

	switch (Backend)
	{
	case ERHIBackend::Vulkan:
		m_Compiler = std::make_unique<DxcShaderCompiler>(ERHIBackend::Vulkan, true);
		break;
	case ERHIBackend::D3D11:
		m_Compiler = std::make_unique<D3DShaderCompiler>(ERHIBackend::D3D11);
		break;
	case ERHIBackend::D3D12:
		m_Compiler = std::make_unique<DxcShaderCompiler>(ERHIBackend::Vulkan, false);
		break;
	}

#if SHADER_HOT_RELOAD
	auto ShaderPath = Paths::ShaderPath().string();
	m_ShaderFileMonitor = std::make_shared<filewatch::FileWatch<std::string>>(ShaderPath, std::regex(R"(.*\.(vert|frag|comp|hlsl|hlsli)$)"),
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

void ShaderLibrary::OnShaderFileModified(const std::filesystem::path& FilePath)
{
	auto Timestamp = Asset::GetLastWriteTime(FilePath);

	std::lock_guard Locker(m_ShaderFileWatchLock);
	auto It = m_ShaderFileWatches.find(FilePath);
	if (It != m_ShaderFileWatches.end())
	{
		if (It->second.Timestamp >= Timestamp)
		{
			return;
		}

		LOG_INFO("ShaderLibrary: Shader file \"{}\" is modified. Queue compilling...", FilePath.string());
		
		It->second.Timestamp = Timestamp;
		for (auto& Hash : It->second.AllPermutations)
		{
			auto PermutationIt = m_ShaderPermutations.find(Hash);
			if (PermutationIt != m_ShaderPermutations.end())
			{
				QueueCompile(PermutationIt->second.SrcShader, Hash, PermutationIt->second.SrcShader.ComputeHash());
			}
		}
	}
	else
	{
		auto IncludeIt = m_IncludeFiles.find(FilePath);
		if (IncludeIt != m_IncludeFiles.end())
		{
			if (IncludeIt->second >= Timestamp)
			{
				return;
			}

			LOG_INFO("ShaderLibrary: Include file \"{}\" is modified. Queue compilling all relative shaders...", FilePath.string());

			IncludeIt->second = Timestamp;
			for (auto& [ShaderPath, Watch] : m_ShaderFileWatches)
			{
				if (Watch.IncludeFiles.find(FilePath) != Watch.IncludeFiles.cend())
				{
					for (auto& Hash : Watch.AllPermutations)
					{
						auto PermutationIt = m_ShaderPermutations.find(Hash);
						if (PermutationIt != m_ShaderPermutations.end())
						{
							QueueCompile(PermutationIt->second.SrcShader, Hash, PermutationIt->second.SrcShader.ComputeHash());
						}
					}
				}
			}
		}
	}
}

void ShaderLibrary::RegisterShaderFileWatch(const Shader& InShader, size_t Hash)
{
	tf::Async([this, &InShader, Hash]() {
		auto& ShaderFilePath = InShader.GetSourceFilePath();

		std::lock_guard Locker(m_ShaderFileWatchLock);
		auto It = m_ShaderFileWatches.find(ShaderFilePath);
		if (It == m_ShaderFileWatches.end())
		{
			It = m_ShaderFileWatches.emplace(ShaderFilePath, ShaderFileWatches()).first;
			It->second.Timestamp = InShader.GetTimestamp();
			It->second.IncludeFiles = ParseIncludeFiles(ShaderFilePath);
			for (auto& Path : It->second.IncludeFiles)
			{
				m_IncludeFiles.emplace(Path, Asset::GetLastWriteTime(Path));
			}
		}

		It->second.AllPermutations.emplace(Hash);
	});
}

std::unordered_set<std::filesystem::path> ShaderLibrary::ParseIncludeFiles(const std::filesystem::path& ShaderFilePath)
{
	std::unordered_set<std::filesystem::path> IncludeFiles;

	if (std::filesystem::exists(ShaderFilePath))
	{
		std::ifstream ShaderFile(ShaderFilePath);
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

void ShaderLibrary::QueueCompile(const Shader& InShader, size_t BaseHash, size_t TimestampHash)
{
	tf::Async([this, BaseHash, TimestampHash, &InShader]() {
		if (RegisterCompileTask(TimestampHash))
		{
			auto& MetaData = InShader.GetMetaData();
			auto& RawData = MetaData.GetRawData(AssetType::EContentsType::Text);

			const time_t Timestamp = MetaData.GetLastWriteTime();
			const auto FileName = InShader.GetName().string();
			const auto SourceCode = reinterpret_cast<char*>(RawData.Data.get());
			const auto Size = RawData.Size;

			auto Blob = m_Compiler->Compile(FileName.c_str(), SourceCode, Size, InShader.GetEntryPoint(), InShader.GetStage(), InShader);
			auto Binary = std::make_shared<ShaderBinary>(FileName, m_Backend, InShader.GetStage(), Timestamp, BaseHash, std::move(Blob));
			Binary->Save(true);
			AddBinary(InShader, Binary);

			DeregisterCompileTask(TimestampHash);
		}
	});
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

const RHIShader* ShaderLibrary::GetShaderModule(const Shader& InShader)
{
	auto BaseHash = InShader.ComputeBaseHash();

	{
		std::lock_guard Locker(m_ShaderPermutationLock);
		auto It = m_ShaderPermutations.find(BaseHash);
		if (It != m_ShaderPermutations.cend())
		{
			return It->second.GetOrCreateRHI(m_Device);
		}
	}

#if SHADER_HOT_RELOAD
	RegisterShaderFileWatch(InShader, BaseHash);
#endif

	{
		auto CacheBinaryPath = ShaderBinary::GetUniquePath(InShader.GetName().string(), m_Backend, BaseHash);
		if (std::filesystem::exists(CacheBinaryPath))
		{
			auto Binary = ShaderBinary::Load(CacheBinaryPath);
			AddBinary(InShader, Binary);
		}
	}

	QueueCompile(InShader, BaseHash, InShader.ComputeHash());
	return nullptr;
}

void ShaderLibrary::AddBinary(const Shader& InShader, std::shared_ptr<ShaderBinary>& Binary)
{
	if (Binary && Binary->IsValid())
	{
		std::lock_guard Locker(m_ShaderPermutationLock);
		auto It = m_ShaderPermutations.find(Binary->GetHash());
		if (It == m_ShaderPermutations.cend())
		{
			It = m_ShaderPermutations.emplace(std::make_pair(Binary->GetHash(), ShaderPermutation(InShader))).first;
		}

		auto& Permutation = It->second;
		if (Permutation.Binary)
		{
			m_MemorySize.fetch_sub(Permutation.Binary->GetSize(), std::memory_order_release);
			Permutation.Module.reset();
			Permutation.Binary.reset();
		}

		std::swap(Permutation.Binary, Binary);
		Permutation.GetOrCreateRHI(m_Device);
		m_MemorySize.fetch_add(Binary->GetSize(), std::memory_order_relaxed);
	}
}

ShaderLibrary::~ShaderLibrary()
{
	m_Compiler.reset();
	m_ShaderFileMonitor.reset();
}