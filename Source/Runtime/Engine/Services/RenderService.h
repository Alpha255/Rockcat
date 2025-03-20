#pragma once

#include "Core/Module.h"
#include "Engine/RHI/RHIBackend.h"

class RenderService : public IService<RenderService>
{
public:
	void OnStartup() override final;

	void OnShutdown() override final;

	RHIBackend* GetBackend(ERHIBackend Backend);
private:
	RHIBackend* GetOrCreateBackend(ERHIBackend Backend);

	Array<std::unique_ptr<RHIBackend>, ERHIBackend> m_Backends;

#if !ENABLE_MULTI_RENDERER
	ERHIBackend m_Backend = ERHIBackend::Num;
#endif
};