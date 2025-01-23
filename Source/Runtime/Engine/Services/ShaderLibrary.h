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
protected:
	IShaderCompiler& GetCompiler(ERHIBackend RHI)
	{
		assert(RHI < ERHIBackend::Num && RHI > ERHIBackend::Software);
		return *m_Compilers[RHI];
	}

	bool RegisterCompileTask(ERHIBackend RHI, size_t Hash);
	void DeregisterCompileTask(ERHIBackend RHI, size_t Hash);

	void OnShaderFileModified(const std::string& FilePath);

	void LoadCache();

	void DoCompile(Shader& InShader, ERHIBackend RHI);
private:
	Array<std::unique_ptr<IShaderCompiler>, ERHIBackend> m_Compilers;
	Array<std::set<size_t>, ERHIBackend> m_CompilingTasks;
	std::shared_ptr<filewatch::FileWatch<std::string>> m_ShaderFileMonitor;
	std::map<size_t, Array<std::shared_ptr<ShaderBinary>, ERHIBackend>> m_Binaries;
	std::mutex m_Lock;
};