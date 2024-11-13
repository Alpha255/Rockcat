#pragma once

#include "Core/Module.h"
#include "Engine/RHI/RHIInterface.h"

class RenderService : public IService<RenderService>
{
public:
	void OnStartup() override final;

	void OnShutdown() override final;

	RHIInterface& GetBackend(ERenderHardwareInterface RHIType) { return *m_RHIs[static_cast<size_t>(RHIType)]; }
private:
	void InitializeRHI(const GraphicsSettings& GfxSettings);

	std::array<std::unique_ptr<RHIInterface>, (size_t)ERenderHardwareInterface::Num> m_RHIs;
};