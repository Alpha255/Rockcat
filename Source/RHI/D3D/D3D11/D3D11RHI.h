#pragma once

#include "Colorful/D3D/D3D11/D3D11Device.h"

NAMESPACE_START(Gfx)

class D3D11Renderer final : public IRenderer
{
public:
	void initialize(uint64_t windowHandle, uint32_t width, uint32_t height, bool8_t fullscreen, bool8_t vSync) override final;

	void toggleFullScreen(bool8_t fullscreen) override final;
	void toggleVSync(bool8_t VSync) override final;

	IDevice* device() override final
	{
		return m_Device.get();
	}

	void finalize() override;
protected:
private:
	std::unique_ptr<D3D11Device> m_Device;
};

NAMESPACE_END(Gfx)