#pragma once

#include "Core/Module.h"
#include "Engine/Asset/ShaderCompiler.h"

namespace filewatch
{
	template<class T> class FileWatch;
};

class ShaderLibrary : public IService<ShaderLibrary>
{
public:
	void OnStartup() override final;

	void OnShutdown() override final;

	const RHIShader* GetShaderModule(const Shader& InShader, ERHIBackend Backend);
protected:
	friend struct ShaderCompileTask;

	IShaderCompiler& GetCompiler(ERHIBackend Backend)
	{
		assert(Backend < ERHIBackend::Num && Backend > ERHIBackend::Software);
		return *m_Compilers[Backend];
	}

	void AddBinary(const std::shared_ptr<ShaderBinary>& Binary);

	bool RegisterCompileTask(ERHIBackend Backend, size_t Hash);
	void DeregisterCompileTask(ERHIBackend Backend, size_t Hash);

	void OnShaderFileModified(const std::filesystem::path& FilePath);

	void LoadCache();

	void QueueCompile(const Shader& InShader, ERHIBackend Backend, size_t BaseHash, size_t TimestampaHash);
private:
	struct ShaderObject
	{
		std::shared_ptr<ShaderBinary> Binary;
		RHIShaderPtr Module;

		const RHIShader* GetOrCreateModule(ERHIBackend Backend);
	};

	struct ShaderFileWatch
	{
		const Shader& Target;
		Array<bool, ERHIBackend> Backends;

		ShaderFileWatch(const Shader& InShader)
			: Target(InShader)
		{
		}
	};

	void RegisterShaderFileWatch(const Shader& InShader, ERHIBackend Backend, size_t Hash);

	Array<std::unique_ptr<IShaderCompiler>, ERHIBackend> m_Compilers;
	Array<std::set<size_t>, ERHIBackend> m_CompilingTasks;
	std::shared_ptr<filewatch::FileWatch<std::string>> m_ShaderFileMonitor;
	std::unordered_map<size_t, Array<ShaderObject, ERHIBackend>> m_ShaderObjects;
	std::unordered_map<std::filesystem::path, std::unordered_map<size_t, std::shared_ptr<ShaderFileWatch>>> m_ShadersToMonitor;
	std::mutex m_Lock;
	std::mutex m_CompileTaskLock;
	std::atomic<size_t> m_MemorySize = 0u;
};