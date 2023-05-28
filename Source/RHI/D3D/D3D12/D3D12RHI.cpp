#include "Colorful/D3D/D3D12/D3D12Renderer.h"
#include "Colorful/D3D/D3D12/D3D12Device.h"
#include "Colorful/D3D/D3D12/D3D12Swapchain.h"

NAMESPACE_START(RHI)

DECLARE_D3D_HWOBJECT(D3D12Debug0, ID3D12Debug)
DECLARE_D3D_HWOBJECT(D3D12Debug1, ID3D12Debug1)
DECLARE_D3D_HWOBJECT(D3D12Debug2, ID3D12Debug2)
DECLARE_D3D_HWOBJECT(D3D12Debug3, ID3D12Debug3)

D3D12Renderer::D3D12Renderer(const RenderSettings* Settings, uint64_t WindowHandle)
	: IRenderer(Settings)
{
	assert(Settings && WindowHandle);

#if defined(_DEBUG)
	D3D12Debug0 DebugLayer0;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(DebugLayer0.Reference()))))
	{
		D3D12Debug3 DebugLayer3;
		if (SUCCEEDED(DebugLayer0->QueryInterface(DebugLayer3.Reference())))
		{
			DebugLayer3->SetEnableGPUBasedValidation(true);
			DebugLayer3->SetEnableSynchronizedCommandQueueValidation(true);
		}
		else
		{
			D3D12Debug1 DebugLayer1;
			if (SUCCEEDED(DebugLayer0->QueryInterface(DebugLayer1.Reference())))
			{
				DebugLayer1->SetEnableGPUBasedValidation(true);
			}
		}
		DebugLayer0->EnableDebugLayer();
	}
#endif

	m_DxgiFactory = std::make_unique<DxgiFactory>(DebugLayer0.IsValid());

	m_Device = std::make_unique<D3D12Device>(m_DxgiFactory.get());

	m_Swapchain = std::make_unique<D3D12Swapchain>(
		m_DxgiFactory.get(),
		m_Device.get(),
		WindowHandle,
		Settings->Resolution.Width,
		Settings->Resolution.Height,
		Settings->FullScreen,
		Settings->VSync,
		false);
}

IDevice* D3D12Renderer::Device()
{
	return m_Device.get();
}

D3D12Renderer::~D3D12Renderer()
{
}

#if defined(DYNAMIC_LIBRARY)
	extern "C"
	{
		EXPORT_API void CreateRenderer(IRendererPtr& RendererPtr, const RenderSettings* Settings)
		{
			RendererPtr = std::make_unique<D3D12Renderer>(Settings);
		}
	}
#endif

NAMESPACE_END(Gfx)