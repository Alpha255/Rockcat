#pragma once

#include "RHI/RHIBackend.h"
#include "RHI/D3D/D3D11/D3D11Types.h"

class D3D11RHI final : public RHIBackend
{
public:
	~D3D11RHI();

	ERHIDeviceType GetType() const override final { return ERHIDeviceType::D3D11; }

	RHIDevice& GetDevice() override final;
protected:
	void Initialize() override final;
private:
	std::unique_ptr<class D3D11Device> m_Device;
};