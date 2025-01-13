#pragma once

#include "Core/Module.h"
#include "Engine/Asset/ShaderCompiler.h"

namespace filewatch
{
	template<class T> class FileWatch;
};

class ShaderCompileService : public IService<ShaderCompileService>
{
public:
	void OnStartup() override final;

	void OnShutdown() override final;

	void Compile(ShaderAsset& Shader, ERHIBackend RHI);
protected:
	IShaderCompiler& GetCompiler(ERHIBackend RHI)
	{
		assert(RHI < ERHIBackend::Num && RHI > ERHIBackend::Software);
		return *m_Compilers[(size_t)RHI];
	}

	bool RegisterCompileTask(ERHIBackend RHI, size_t Hash);
	void DeregisterCompileTask(ERHIBackend RHI, size_t Hash);

	void OnShaderFileModified(const std::string& FilePath);
private:
	std::array<std::unique_ptr<IShaderCompiler>, (size_t)ERHIBackend::Num> m_Compilers;
	std::array<std::set<size_t>, (size_t)ERHIBackend::Num> m_CompilingTasks;
	std::shared_ptr<filewatch::FileWatch<std::string>> m_ShaderFileMonitor;
};