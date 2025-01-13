#pragma once

#include "Core/Module.h"
#include "Engine/RHI/RHIInterface.h"

class RenderService : public IService<RenderService>
{
public:
	void OnStartup() override final;

	void OnShutdown() override final;

	RHIInterface& GetBackend(ERHIBackend RHI) 
	{
		assert(m_Backends[(size_t)RHI]);
		return *m_Backends[(size_t)RHI];
	}
private:
	void InitializeRHI(const GraphicsSettings& GfxSettings);

	std::array<std::unique_ptr<RHIInterface>, (size_t)ERHIBackend::Num> m_Backends;
};