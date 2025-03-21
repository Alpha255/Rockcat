#include "Engine/Services/ShaderLibrary.h"
#include "Engine/Async/Task.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/Services/TaskFlowService.h"
#include "Engine/Paths.h"
#include "Engine/RHI/RHIPipeline.h"
#include "Engine/Services/RenderService.h"
#pragma warning(disable:4068)
#include <Submodules/filewatch/FileWatch.hpp>
#pragma warning(default:4068)

struct ShaderCompileTask : public Task
{
	ShaderCompileTask(const Shader& InShader, IShaderCompiler& InCompiler)
		: Shader(InShader)
		, Compiler(InCompiler)
		, Task(std::move(std::string("ShaderCompileTask|") + InShader.GetSourceFilePath().string()), EPriority::High)
	{
	}

	void Execute() override final
	{
		auto& MetaData = Shader.GetMetaData();
		auto& RawData = MetaData.GetRawData(AssetType::EContentsType::Text);

		const auto Backend = Compiler.GetBackend();
		const time_t Timestamp = MetaData.GetLastWriteTime();
		const size_t Hash = Shader.ComputeHash();
		const auto FileName = Shader.GetName().string();
		const auto SourceCode = reinterpret_cast<char*>(RawData.Data.get());
		const auto Size = RawData.Size;

		auto Blob = Compiler.Compile(FileName.c_str(), SourceCode, Size, Shader.GetEntryPoint(), Shader.GetStage(), Shader);
		auto Binary = std::make_shared<ShaderBinary>(FileName, Backend, Shader.GetStage(), Timestamp, Hash, std::move(Blob));
		Binary->Save(true);
		ShaderLibrary::Get().AddBinary(Binary);
	}

	const Shader& Shader;
	IShaderCompiler& Compiler;
};

const RHIShader* ShaderLibrary::ShaderObject::GetOrCreateModule(ERHIBackend Backend)
{
	if (!Module && Binary->IsValid())
	{
		if (auto RenderBackend = RenderService::Get().GetBackend(Backend))
		{
			RHIShaderCreateInfo ShaderCreateInfo;
			ShaderCreateInfo.SetStage(Binary->GetStage())
				.SetShaderBinary(Binary.get())
				.SetName(Binary->GetName().string());

			Module = RenderBackend->GetDevice().CreateShader(ShaderCreateInfo);
		}
	}

	return Module.get();
}

void ShaderLibrary::OnStartup()
{
	REGISTER_LOG_CATEGORY(LogShaderCompile);

	m_Compilers[ERHIBackend::Vulkan] = std::make_unique<DxcShaderCompiler>(ERHIBackend::Vulkan, true);
	m_Compilers[ERHIBackend::D3D11] = std::make_unique<D3DShaderCompiler>(ERHIBackend::D3D11);
	m_Compilers[ERHIBackend::D3D12] = std::make_unique<DxcShaderCompiler>(ERHIBackend::Vulkan, false);

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

	LoadCache();
}

void ShaderLibrary::OnShaderFileModified(const std::string& FilePath)
{
	LOG_INFO("{} is modified.", FilePath);
}

void ShaderLibrary::RegisterShaderFileWatch(const Shader& InShader, ERHIBackend Backend, size_t Hash)
{
	auto ShaderFilePath = InShader.GetSourceFilePath().string();
	auto It = m_ShadersToMonitor.find(ShaderFilePath);
	if (It == m_ShadersToMonitor.end())
	{
		It = m_ShadersToMonitor.emplace(ShaderFilePath, std::unordered_map<size_t, std::shared_ptr<ShaderFileWatch>>()).first;
		auto WatchIt = It->second.emplace(Hash, std::make_shared<ShaderFileWatch>(InShader)).first;
		WatchIt->second->Backends[Backend] = true;
	}
}

void ShaderLibrary::QueueCompile(const Shader& InShader, ERHIBackend Backend, size_t BaseHash, size_t TimestampaHash)
{
	assert(Backend < ERHIBackend::Num);

	tf::Async([this, Backend, BaseHash, TimestampaHash, &InShader]() {
		if (RegisterCompileTask(Backend, TimestampaHash))
		{
			auto& Compiler = GetCompiler(Backend);

			auto& MetaData = InShader.GetMetaData();
			auto& RawData = MetaData.GetRawData(AssetType::EContentsType::Text);

			const time_t Timestamp = MetaData.GetLastWriteTime();
			const auto FileName = InShader.GetName().string();
			const auto SourceCode = reinterpret_cast<char*>(RawData.Data.get());
			const auto Size = RawData.Size;

			auto Blob = Compiler.Compile(FileName.c_str(), SourceCode, Size, InShader.GetEntryPoint(), InShader.GetStage(), InShader);
			auto Binary = std::make_shared<ShaderBinary>(FileName, Backend, InShader.GetStage(), Timestamp, BaseHash, std::move(Blob));
			Binary->Save(true);
			AddBinary(Binary);

			DeregisterCompileTask(Backend, TimestampaHash);
		}
	});
}

bool ShaderLibrary::RegisterCompileTask(ERHIBackend Backend, size_t Hash)
{
	assert(Backend < ERHIBackend::Num);

	std::lock_guard Lock(m_CompileTaskLock);
	if (m_CompilingTasks[Backend].find(Hash) != m_CompilingTasks[Backend].cend())
	{
		return false;
	}

	m_CompilingTasks[Backend].insert(Hash);
	return true;
}

void ShaderLibrary::DeregisterCompileTask(ERHIBackend Backend, size_t Hash)
{
	assert(Backend < ERHIBackend::Num);

	std::lock_guard Lock(m_CompileTaskLock);
	m_CompilingTasks[Backend].erase(Hash);
}

void ShaderLibrary::LoadCache()
{
	if (std::filesystem::exists(Paths::ShaderBinaryCachePath()))
	{
		tf::Async([this]() {
			for (auto& Entry : std::filesystem::recursive_directory_iterator(Paths::ShaderBinaryCachePath()))
			{
				if (Entry.is_regular_file())
				{
					auto Binary = ShaderBinary::Load(Entry.path());
					AddBinary(Binary);
				}
			}
		});
	}
}

const RHIShader* ShaderLibrary::GetShaderModule(const Shader& InShader, ERHIBackend Backend)
{
	auto BaseHash = InShader.ComputeBaseHash();

	std::lock_guard Locker(m_Lock);
	auto It = m_ShaderObjects.find(BaseHash);
	if (It != m_ShaderObjects.cend())
	{
		return It->second[Backend].GetOrCreateModule(Backend);
	}

#if SHADER_HOT_RELOAD
#endif

	QueueCompile(InShader, Backend, BaseHash, InShader.ComputeHash());
	// ### TODO: Get fallback
	return nullptr;
}

void ShaderLibrary::AddBinary(const std::shared_ptr<ShaderBinary>& Binary)
{
	if (Binary && Binary->IsValid())
	{
		std::lock_guard Locker(m_Lock);
		auto It = m_ShaderObjects.find(Binary->GetHash());
		if (It == m_ShaderObjects.cend())
		{
			It = m_ShaderObjects.emplace(std::make_pair(Binary->GetHash(), Array<ShaderObject, ERHIBackend>())).first;
		}

		auto& Object = It->second[Binary->GetBackend()];
		if (Object.Binary)
		{
			if (Binary->GetTimestamp() > Object.Binary->GetTimestamp()) // Can this ensure the thread safety, consider of when you edit a shader file and save it quickyly ?
			{
				m_MemorySize.fetch_sub(Object.Binary->GetSize(), std::memory_order_release);
				Object.Module.reset();
			}
			else
			{
				return;
			}
		}

		Object.Binary = Binary;
		m_MemorySize.fetch_add(Binary->GetSize(), std::memory_order_relaxed);
	}
}

void ShaderLibrary::OnShutdown()
{
	for (auto& Compiler : m_Compilers)
	{
		Compiler.reset();
	}

	m_ShaderFileMonitor.reset();
}