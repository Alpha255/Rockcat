#include "Engine/RHI/RHIInterface.h"

std::array<const GraphicsSettings*, (size_t)ERHIBackend::Num> RHIInterface::s_GraphicsSettings;

RHIInterface::RHIInterface(const GraphicsSettings* GfxSettings)
{
	// TODO: How to handle multi window use same render backend
	assert(GfxSettings && GfxSettings->Interface < ERHIBackend::Num);
	s_GraphicsSettings[(size_t)GfxSettings->Interface] = GfxSettings;
}

const GraphicsSettings& RHIInterface::GetGraphicsSettings(ERHIBackend Interface)
{
	assert(Interface < ERHIBackend::Num);
	return *s_GraphicsSettings[(size_t)Interface];
}