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

	const ShaderBinary* GetBinary(Shader& InShader, ERHIBackend Backend);

	void AddBinary(const std::shared_ptr<ShaderBinary>& Binary);
protected:
	IShaderCompiler& GetCompiler(ERHIBackend Backend)
	{
		assert(Backend < ERHIBackend::Num && Backend > ERHIBackend::Software);
		return *m_Compilers[Backend];
	}

	bool RegisterCompileTask(ERHIBackend Backend, size_t Hash);
	void DeregisterCompileTask(ERHIBackend Backend, size_t Hash);

	void OnShaderFileModified(const std::string& FilePath);

	void LoadCache();

	void DoCompile(Shader& InShader, ERHIBackend Backend, size_t Hash);
private:
	Array<std::unique_ptr<IShaderCompiler>, ERHIBackend> m_Compilers;
	Array<std::set<size_t>, ERHIBackend> m_CompilingTasks;
	std::shared_ptr<filewatch::FileWatch<std::string>> m_ShaderFileMonitor;
	std::map<size_t, Array<std::shared_ptr<ShaderBinary>, ERHIBackend>> m_Binaries;
	std::mutex m_Lock;
	std::mutex m_CompileTaskLock;
};