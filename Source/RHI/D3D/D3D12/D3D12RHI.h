#pragma once

#include "Engine/RHI/RHIBackend.h"
#include "RHI/D3D/D3D12/D3D12Types.h"

class D3D12RHI final : public RHIBackend
{
public:
	~D3D12RHI();

	ERHIBackend GetType() const override final { return ERHIBackend::D3D12; }

	RHIDevice& GetDevice() override final;
protected:
	void InitializeGraphicsDevice() override final;
private:
	std::unique_ptr<DxgiFactory> m_DxgiFactory;
	std::unique_ptr<class D3D12Device> m_Device;
	//std::unique_ptr<class D3D12Swapchain> m_Swapchain;
};