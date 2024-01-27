#pragma once

#include "Runtime/Core/Module.h"
#include "Runtime/Engine/RHI/RHIInterface.h"

class RenderService : public IService<RenderService>
{
public:
	void InitializeRHI(const GraphicsSettings& GfxSettings);

	void OnShutdown() override final;

	RHIInterface& GetRHIInterface(ERenderHardwareInterface RHIType) { return *m_RHIs[static_cast<size_t>(RHIType)]; }
private:
	std::array<std::unique_ptr<RHIInterface>, (size_t)ERenderHardwareInterface::Num> m_RHIs;
};