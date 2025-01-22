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
		assert(m_Backends[RHI]);
		return *m_Backends[RHI];
	}
private:
	void InitializeRHI(const GraphicsSettings& GfxSettings);

	Array<std::unique_ptr<RHIInterface>, ERHIBackend> m_Backends;
};