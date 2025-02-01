#include "Engine/Services/ShaderLibrary.h"
#include "Engine/Async/Task.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/Services/TaskFlowService.h"
#include "Engine/Paths.h"
#pragma warning(disable:4068)
#include <Submodules/filewatch/FileWatch.hpp>
#pragma warning(default:4068)

struct ShaderCompileTask : public Task
{
	ShaderCompileTask(Shader& InShader, IShaderCompiler& InCompiler)
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
		auto Binary = std::make_shared<ShaderBinary>(FileName, Backend, Timestamp, Hash, std::move(Blob));
		Binary->Save(true);
		ShaderLibrary::Get().AddBinary(Binary);
	}

	Shader& Shader;
	IShaderCompiler& Compiler;
};

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

void ShaderLibrary::DoCompile(Shader& InShader, ERHIBackend Backend, size_t Hash)
{
	assert(Backend < ERHIBackend::Num);

	tf::Async([this, Backend, Hash, &InShader]() {
		if (RegisterCompileTask(Backend, Hash))
		{
			auto& Compiler = GetCompiler(Backend);

			auto& MetaData = InShader.GetMetaData();
			auto& RawData = MetaData.GetRawData(AssetType::EContentsType::Text);

			const time_t Timestamp = MetaData.GetLastWriteTime();
			const auto FileName = InShader.GetName().string();
			const auto SourceCode = reinterpret_cast<char*>(RawData.Data.get());
			const auto Size = RawData.Size;

			auto Blob = Compiler.Compile(FileName.c_str(), SourceCode, Size, InShader.GetEntryPoint(), InShader.GetStage(), InShader);
			auto Binary = std::make_shared<ShaderBinary>(FileName, Backend, Timestamp, Hash, std::move(Blob));
			Binary->Save(true);
			AddBinary(Binary);

			DeregisterCompileTask(Backend, Hash);
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

const ShaderBinary* ShaderLibrary::GetBinary(Shader& InShader, ERHIBackend Backend)
{
	auto Hash = InShader.ComputeHash();

	std::lock_guard Locker(m_Lock);
	auto It = m_Binaries.find(Hash);
	if (It != m_Binaries.cend())
	{
		if (It->second[Backend])
		{
			return It->second[Backend].get();
		}
	}

	DoCompile(InShader, Backend, Hash);
	return nullptr;
}

void ShaderLibrary::AddBinary(const std::shared_ptr<ShaderBinary>& Binary)
{
	if (Binary && Binary->IsValid())
	{
		std::lock_guard Locker(m_Lock);
		auto It = m_Binaries.find(Binary->GetHash());
		if (It == m_Binaries.cend())
		{
			It = m_Binaries.emplace(std::make_pair(Binary->GetHash(), Array<std::shared_ptr<ShaderBinary>, ERHIBackend>())).first;
		}
		It->second[Binary->GetBackend()] = Binary;
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