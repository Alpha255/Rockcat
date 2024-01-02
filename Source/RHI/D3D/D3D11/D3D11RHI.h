#pragma once

#include "Runtime/Engine/RHI/RHIInterface.h"
#include "RHI/D3D/D3D11/D3D11Types.h"

class D3D11RHI final : public RHIInterface
{
public:
	D3D11RHI(const GraphicsSettings* GfxSettings);

	~D3D11RHI();

	ERenderHardwareInterface GetRHIType() const override final { return ERenderHardwareInterface::D3D11; }

	RHIDevice& GetDevice() override final;
protected:
	void InitializeGraphicsDevices() override final;
private:
	std::unique_ptr<class D3D11Device> m_Device;
};