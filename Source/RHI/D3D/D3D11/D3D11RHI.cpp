#include "RHI/D3D/D3D11/D3D11RHI.h"
#include "RHI/D3D/D3D11/D3D11Device.h"

D3D11RHI::~D3D11RHI()
{
}

RHIDevice& D3D11RHI::GetDevice()
{
	return *m_Device;
}

void D3D11RHI::InitializeGraphicsDevice()
{
}
