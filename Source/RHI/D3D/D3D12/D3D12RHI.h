#pragma once

#include "Runtime/Engine/RHI/RHIInterface.h"
#include "RHI/D3D/DXGI_Interface.h"

class D3D12Renderer final : public RHIInterface
{
public:
	D3D12Renderer(const RenderSettings* Settings, uint64_t WindowHandle);

	~D3D12Renderer();

	IDevice* Device() override final;

	void Present() override final {}

	void OnWindowResized(uint32_t Width, uint32_t Height) override final 
	{
		(void)Width;
		(void)Height;
	}

	IFrameBuffer* SwapchainFrameBuffer() override final 
	{
		return nullptr;
	}
protected:
private:
	std::unique_ptr<DxgiFactory> m_DxgiFactory;
	std::unique_ptr<class D3D12Device> m_Device;
	std::unique_ptr<class D3D12Swapchain> m_Swapchain;
};