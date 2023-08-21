#pragma once

#include "Runtime/Core/Module.h"
#include "Runtime/Engine/RHI/RHIInterface.h"
#include "Runtime/Engine/Application/GraphicsSettings.h"

class RenderService : public IService<RenderService>
{
public:
	void InitializeRHI(const GraphicsSettings& GfxSettings);

	void OnShutdown() override final;
private:
	std::array<std::unique_ptr<RHIInterface>, (size_t)ERenderHardwareInterface::Null> m_RHIs;
};