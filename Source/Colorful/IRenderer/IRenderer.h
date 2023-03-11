#pragma once

#include "Colorful/IRenderer/IDevice.h"
#include "Colorful/IRenderer/RenderSettings.h"
#include "Colorful/IRenderer/Profiler.h"
#include "Colorful/IRenderer/RenderGraph/IFrameGraph.h"

NAMESPACE_START(RHI)

#define CREATE_RENDERER_FUNC "CreateRenderer"

enum class ERenderer
{
	Null,
	D3D11,
	D3D12,
	Vulkan,
	Software
};

class IRenderer
{
public:
	IRenderer(const RenderSettings* Settings)
		: m_Settings(Settings)
	{
	}
	
	virtual ~IRenderer() = default;

	static void Create(const RenderSettings* Settings, uint64_t WindowHandle);

	static void Destroy();

	static IRenderer& Get();

	virtual IDevice* Device() = 0;

	virtual void AdvanceNextFrame() {}

	virtual void Present() = 0;

	virtual void OnWindowResized(uint32_t Width, uint32_t Height) = 0;

	virtual IFrameBuffer* SwapchainFrameBuffer() = 0;

	virtual ERenderer Type() const
	{
		return ERenderer::Null;
	}

	virtual const char8_t* Name() const
	{
		return "Null";
	}

	const RenderSettings* Settings() const
	{
		return m_Settings;
	}
protected:
	const RenderSettings* m_Settings = nullptr;
private:
	static std::unique_ptr<IRenderer> Instance;
};

NAMESPACE_END(RHI)
