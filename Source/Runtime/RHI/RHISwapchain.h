#pragma once

#include "RHI/RHIRenderStates.h"

class RHISwapchain
{
public:
	RHISwapchain(const RHISwapchainCreateInfo& CreateInfo)
		: m_VSync(CreateInfo.VSync)
		, m_Fullscreen(CreateInfo.Fullscreen)
		, m_Width(CreateInfo.Width)
		, m_Height(CreateInfo.Height)
		, m_NumBackBuffer(1u)
		, m_BackBufferIndex(0u)
		, m_WindowHandle(CreateInfo.WindowHandle)
	{
	}

	inline uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() const { return m_Height; }
	inline uint32_t GetNumBackBuffer() const { return m_NumBackBuffer; }
	inline bool IsVSync() const { return m_VSync; }
	inline bool IsFullscreen() const { return m_Fullscreen; }

	inline RHITexture* GetBackBuffer() const
	{
		assert(m_BackBuffers.size() == m_NumBackBuffer);
		return m_BackBuffers[m_BackBufferIndex].get();
	}

	virtual void Resize(uint32_t Width, uint32_t Height) = 0;
	virtual void AdvanceFrame() = 0;
	virtual void Present() = 0;
protected:
	void SetNumBackBuffer(uint32_t Num) { m_NumBackBuffer = Num; }

	bool m_VSync;
	bool m_Fullscreen;
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_NumBackBuffer;
	uint32_t m_BackBufferIndex;
	const void* m_WindowHandle;
	std::vector<RHITexturePtr> m_BackBuffers;
};
