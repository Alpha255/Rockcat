#include "RHI/D3D/D3D11/D3D11RHI.h"

D3D11RHI::D3D11RHI(const GraphicsSettings* GfxSettings)
	: RHIInterface(GfxSettings)
{
	assert(GfxSettings);

	InitializeGraphicsDevices();
}

D3D11RHI::~D3D11RHI()
{
}

void D3D11RHI::InitializeGraphicsDevices()
{
}
