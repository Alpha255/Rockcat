#pragma once

#include "Core/Module.h"

class RenderService : public IService<RenderService>
{
public:
	void OnStartup() override final;
	void OnShutdown() override final;

	inline class RHIBackend& GetBackend() { return *m_Backend; }
private:
	class RHIBackend* m_Backend = nullptr;
};
