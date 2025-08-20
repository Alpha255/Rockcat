#pragma once

#include "Core/Singleton.h"
#include "Asset/ShaderCompiler.h"

namespace filewatch
{
	template<class T> class FileWatch;
};

class ShaderLibrary : public LazySingleton<ShaderLibrary>
{
public:
	~ShaderLibrary();

	const RHIShader* GetShaderModule(const Shader& InShader);
protected:
	ALLOW_ACCESS_LAZY(ShaderLibrary);

	friend struct ShaderCompileTask;

	ShaderLibrary(RHIDevice& Device);

	bool RegisterCompileTask(size_t Hash);
	void DeregisterCompileTask(size_t Hash);

	void OnShaderFileModified(const std::filesystem::path& FilePath);

	void QueueCompile(const Shader& InShader, size_t BaseHash, size_t TimestampHash);
private:
	struct ShaderPermutation
	{
		const Shader& SrcShader;
		std::shared_ptr<ShaderBinary> Binary;
		RHIShaderPtr Module;

		const RHIShader* GetOrCreateRHI(RHIDevice& Device);

		ShaderPermutation(const Shader& InShader)
			: SrcShader(InShader)
		{
		}
	};

	struct ShaderFileWatches
	{
		time_t Timestamp = 0u;
		std::unordered_set<std::filesystem::path> IncludeFiles;
		std::unordered_set<size_t> AllPermutations;
	};

	void AddBinary(const Shader& InShader, std::shared_ptr<ShaderBinary>& Binary);

	void RegisterShaderFileWatch(const Shader& InShader, size_t Hash);

	std::unordered_set<std::filesystem::path> ParseIncludeFiles(const std::filesystem::path& ShaderFilePath);

	RHIDevice& m_Device;

	std::set<size_t> m_CompilingTasks;
	std::unique_ptr<IShaderCompiler> m_Compiler;
	std::shared_ptr<filewatch::FileWatch<std::string>> m_ShaderFileMonitor;
	std::unordered_map<std::filesystem::path, time_t> m_IncludeFiles;
	std::unordered_map<std::filesystem::path, ShaderFileWatches> m_ShaderFileWatches;
	std::unordered_map<size_t, ShaderPermutation> m_ShaderPermutations;
	std::mutex m_ShaderPermutationLock;
	std::mutex m_CompileTaskLock;
	std::mutex m_ShaderFileWatchLock;
	std::atomic<size_t> m_MemorySize = 0u;
	std::string_view m_DeviceName;
};