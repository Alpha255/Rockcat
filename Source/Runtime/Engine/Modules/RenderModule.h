#pragma once

#include "Runtime/Core/Module.h"
#include "Runtime/Engine/RHI/RHIInterface.h"
#include "Runtime/Engine/Application/GraphicsSettings.h"

class RenderModule : public IModule
{
public:
	void InitializeRHI(const GraphicsSettings& GfxSettings);

	void Finalize();
private:
	std::array<std::unique_ptr<RHIInterface>, (size_t)ERenderHardwareInterface::Null> m_RHIs;
};