#pragma once

#include "Engine/RHI/RHIDeclarations.h"

class RHISwapchain
{

};

class RHIViewport
{
public:
	RHIViewport(RHIDevice& Device, const void* WindowHandle, uint32_t Width, uint32_t Height, bool VSync, bool Fullscreen);

	inline uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() const { return m_Height; }
	inline uint32_t GetNumBackBuffer() const { return m_NumBackBuffer; }
	inline bool IsVSync() const { return m_VSync; }
	inline bool IsFullscreen() const { return m_Fullscreen; }

	virtual RHITexture* GetBackBuffer(uint32_t) const { return nullptr; }
	virtual void Resize(uint32_t Width, uint32_t Height) = 0;
	virtual void AdvanceFrame() = 0;
protected:
	void SetNumBackBuffer(uint32_t Num) { m_NumBackBuffer = Num; }

	bool m_VSync;
	bool m_Fullscreen;
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_NumBackBuffer;
	uint32_t m_BackBufferIndex;
	const void* m_WindowHandle;
	RHISwapchainPtr m_Swapchain;
};
