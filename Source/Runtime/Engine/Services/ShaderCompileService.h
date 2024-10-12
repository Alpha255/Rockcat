#pragma once

#include "Core/Module.h"
#include "Engine/Asset/ShaderCompiler.h"

class ShaderCompileService : public IService<ShaderCompileService>
{
public:
	void OnStartup() override final;

	void OnShutdown() override final;

	void Compile(const ShaderAsset& Shader);
protected:
	IShaderCompiler& GetCompiler(ERenderHardwareInterface RHI)
	{
		assert(RHI < ERenderHardwareInterface::Num && RHI > ERenderHardwareInterface::Software);
		return *m_Compilers[(size_t)RHI];
	}

	bool RegisterCompileTask(ERenderHardwareInterface RHI, size_t Hash);
	void DeregisterCompileTask(ERenderHardwareInterface RHI, size_t Hash);
private:
	std::array<std::unique_ptr<IShaderCompiler>, (size_t)ERenderHardwareInterface::Num> m_Compilers;
	std::array<std::set<size_t>, (size_t)ERenderHardwareInterface::Num> m_CompilingTasks;
};