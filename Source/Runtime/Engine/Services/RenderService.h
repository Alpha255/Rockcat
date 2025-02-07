#pragma once

#include "Core/Module.h"
#include "Engine/RHI/RHIBackend.h"

class RenderService : public IService<RenderService>
{
public:
	void OnStartup() override final;

	void OnShutdown() override final;
private:
	std::shared_ptr<RHIBackend> GetOrCreateBackend(ERHIBackend Backend);

	Array<std::shared_ptr<RHIBackend>, ERHIBackend> m_Backends;
};