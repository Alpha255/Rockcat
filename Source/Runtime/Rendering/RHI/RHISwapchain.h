#pragma once

#include "Rendering/RHI/RHIRenderStates.h"

class RHISwapchain
{
public:
	RHISwapchain(const RHISwapchainDesc& Desc)
		: m_VSync(Desc.VSync)
		, m_Fullscreen(Desc.Fullscreen)
		, m_Width(Desc.Width)
		, m_Height(Desc.Height)
		, m_NumBackBuffer(1u)
		, m_BackBufferIndex(0u)
		, m_WindowHandle(Desc.WindowHandle)
	{
	}

	virtual ~RHISwapchain() = default;

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
	virtual void Present() = 0;

	/// Acquires the next back buffer. Must be the first thing a frame does.
	virtual void AdvanceFrame() {}

	/// Records whatever the backend needs right before Present() (e.g. moving the back buffer into
	/// the layout presentation expects). No-op for backends that don't need it.
	virtual void TransitionToPresent(class RHICommandBuffer* CommandBuffer) {}
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

class RHIViewportClient
{
public:
	enum class EFinalOutputMode : uint8_t
	{
		Default,
		Offscreen
	};

	RHIViewportClient(const class Window& InWindow, const struct RenderSettings& InRenderSettings);

	Math::UInt2 GetViewSize() const;

	RHITexture* GetViewSurface() const;

	RHISwapchain* GetSwapchain() const { return m_Swapchain.get(); }

	bool IsStereoRendering() const { return m_IsStereo; }
private:
	const class Window& m_Window;
	RHISwapchainPtr m_Swapchain;
	EFinalOutputMode m_FinalOutputMode = EFinalOutputMode::Default;
	bool m_IsStereo = false;
	RHITexturePtr m_OffscreenViewSurface = nullptr;
};