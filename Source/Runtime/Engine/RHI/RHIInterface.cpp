#include "Engine/RHI/RHIInterface.h"

std::array<const GraphicsSettings*, (size_t)ERenderHardwareInterface::Num> RHIInterface::s_GraphicsSettings;

RHIInterface::RHIInterface(const GraphicsSettings* GfxSettings)
{
	assert(GfxSettings && GfxSettings->RenderHardwareInterface < ERenderHardwareInterface::Num);
	s_GraphicsSettings[static_cast<size_t>(GfxSettings->RenderHardwareInterface)] = GfxSettings;
}

const GraphicsSettings& RHIInterface::GetGraphicsSettings(ERenderHardwareInterface Interface)
{
	assert(Interface < ERenderHardwareInterface::Num);
	return *s_GraphicsSettings[static_cast<size_t>(Interface)];
}