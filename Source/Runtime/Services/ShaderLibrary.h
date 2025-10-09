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

	void CompileShader(const Shader& InShader);
	void QueueCompileShader(const Shader& InShader);
protected:
	ALLOW_ACCESS_LAZY(ShaderLibrary);

	ShaderLibrary();

	bool RegisterCompileTask(size_t Hash);
	void DeregisterCompileTask(size_t Hash);

	void OnShaderFileModified(const std::filesystem::path& Path);
private:

	struct ShaderFileWatches
	{
		time_t Timestamp = 0u;
		std::unordered_set<std::filesystem::path> IncludeFiles;
	};

	void AddBinary(const Shader& InShader, std::shared_ptr<ShaderBinary>& Binary);

	void RegisterShaderFileWatch(const Shader& InShader);

	std::unordered_set<std::filesystem::path> ParseIncludeFiles(const std::filesystem::path& Path);

	RHIDevice& m_Device;

	std::set<size_t> m_CompilingTasks;
	Array<std::unique_ptr<IShaderCompiler>, ERHIDeviceType> m_Compilers;
	std::shared_ptr<filewatch::FileWatch<std::string>> m_ShaderFileMonitor;
	std::unordered_map<std::filesystem::path, time_t> m_IncludeFiles;
	std::unordered_map<std::filesystem::path, ShaderFileWatches> m_ShaderFileWatches;
	std::unordered_map<size_t, ShaderPermutation> m_ShaderPermutations;
	std::mutex m_ShaderPermutationLock;
	std::mutex m_CompileTaskLock;
	std::mutex m_ShaderFileWatchLock;
};