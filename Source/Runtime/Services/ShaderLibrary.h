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
private:
	struct IncludeFileInfo
	{
		time_t LastWriteTime = 0u;
		std::unordered_set<const Shader*> LinkedShaders;
	};

	bool RegisterCompileTask(size_t Hash);
	void DeregisterCompileTask(size_t Hash);

	void OnShaderFileModified(const std::filesystem::path& Path);

	void AddBinary(const Shader& InShader, std::shared_ptr<ShaderBinary>& Binary);

	void RegisterShaderFileWatch(const Shader& InShader);

	std::unordered_set<std::filesystem::path> ParseIncludeFiles(const std::filesystem::path& Path);
	void UpdateIncludeFileInfos(const Shader& InShader);

	std::unordered_set<size_t> m_CompilingTasks;
	Array<std::unique_ptr<IShaderCompiler>, ERHIDeviceType> m_Compilers;
	std::shared_ptr<filewatch::FileWatch<std::string>> m_ShaderFileWatch;
	std::unordered_map<std::filesystem::path, IncludeFileInfo> m_IncludeFileInfos;
};