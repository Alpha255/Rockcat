#pragma once

#include "Core/Singleton.h"
#include "Asset/ShaderCompiler.h"

namespace filewatch
{
	template<class T> class FileWatch;
};

class ShaderLibrary : public IService<ShaderLibrary>
{
public:
	virtual void Initialize() override final;
	virtual void Finalize() override final;

	inline void RegisterActiveCompiler(ERHIDeviceType DeviceType)
	{
		m_ActiveCompilers.insert(DeviceType);
	}

	void Compile(Shader& InShader, ERHIDeviceType DeviceType);
	void QueueCompile(Shader& InShader, ERHIDeviceType DeviceType);
protected:
private:
	using LinkedShaders = std::unordered_set<Shader*>;

	bool RegisterCompileTask(size_t Hash);
	void DeregisterCompileTask(size_t Hash);

	void OnShaderFileModified(const std::filesystem::path& Path);

	void RegisterShader(Shader& InShader);

	std::unordered_set<std::filesystem::path> ParseIncludeFiles(const Shader& InShader);

	inline IShaderCompiler* GetCompiler(ERHIDeviceType DeviceType) { return m_Compilers[DeviceType].get(); }

	std::unordered_set<size_t> m_CompilingTasks;
	std::unordered_set<ERHIDeviceType> m_ActiveCompilers;
	Array<std::unique_ptr<IShaderCompiler>, ERHIDeviceType> m_Compilers;
	std::shared_ptr<filewatch::FileWatch<std::string>> m_ShaderFileWatch;
	std::unordered_map<std::filesystem::path, LinkedShaders> m_HeaderFileInfos;
	std::unordered_map<std::filesystem::path, Shader*> m_Shaders;

	std::mutex m_CompileTaskLock;
	std::mutex m_ShaderLock;
};