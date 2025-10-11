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
	ShaderLibrary(ERHIDeviceType DeviceType);
	~ShaderLibrary();

	void CompileShader(const Shader& InShader);
	void QueueCompileShader(const Shader& InShader);
protected:
private:
	using LinkedShaders = std::unordered_set<const Shader*>;

	bool RegisterCompileTask(size_t Hash);
	void DeregisterCompileTask(size_t Hash);

	void OnShaderFileModified(const std::filesystem::path& Path);

	void AddBinary(const Shader& InShader, std::shared_ptr<ShaderBinary>& Binary);

	void RegisterShader(const Shader& InShader);

	std::unordered_set<std::filesystem::path> ParseIncludeFiles(const Shader& InShader);

	std::unordered_set<size_t> m_CompilingTasks;
	std::unique_ptr<IShaderCompiler> m_Compiler;
	std::shared_ptr<filewatch::FileWatch<std::string>> m_ShaderFileWatch;
	std::unordered_map<std::filesystem::path, LinkedShaders> m_HeaderFileInfos;
	std::unordered_map<std::filesystem::path, const Shader*> m_Shaders;

	std::mutex m_CompileTaskLock;
	std::mutex m_ShaderLock;
};