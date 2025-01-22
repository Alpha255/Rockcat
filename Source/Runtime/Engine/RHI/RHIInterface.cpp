#include "Engine/RHI/RHIInterface.h"

Array<const GraphicsSettings*, ERHIBackend> RHIInterface::s_GraphicsSettings;

RHIInterface::RHIInterface(const GraphicsSettings* GfxSettings)
{
	// TODO: How to handle multi window use same render backend
	assert(GfxSettings && GfxSettings->Interface < ERHIBackend::Num);
	s_GraphicsSettings[GfxSettings->Interface] = GfxSettings;
}

const GraphicsSettings& RHIInterface::GetGraphicsSettings(ERHIBackend Interface)
{
	assert(Interface < ERHIBackend::Num);
	return *s_GraphicsSettings[Interface];
}