#pragma once

#include "Colorful/IRenderer/IRenderer.h"
#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(RHI)

class D3D12Renderer final : public IRenderer
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

NAMESPACE_END(RHI)