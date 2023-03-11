#include "Colorful/IRenderer/IRenderer.h"
#include "Colorful/Vulkan/VulkanRenderer.h"
#include "Colorful/D3D/D3D12/D3D12Renderer.h"
#include "Colorful/IRenderer/StagingBufferAllocator.h"
#include "Runtime/Asset/AssetDatabase.h"

NAMESPACE_START(RHI)

std::unique_ptr<IRenderer> IRenderer::Instance;

void IRenderer::Create(const RenderSettings* Settings, uint64_t WindowHandle)
{
	assert(Settings && WindowHandle);

	std::string_view RendererName(Settings->RendererName());
	if (RendererName == "Vulkan")
	{
		Instance = std::make_unique<VulkanRenderer>(Settings, WindowHandle);
	}
	else if (RendererName == "D3D12")
	{
		assert(0);
		Instance = std::make_unique<D3D12Renderer>(Settings, WindowHandle);
	}
	else
	{
		assert(0);
	}

	AssetDatabase::Create();

	StagingBufferAllocator::Create(Instance->Device());

	///Gfx::AssetDatabase::instance().loadDefaultAssets();
}

IRenderer& IRenderer::Get()
{
	assert(Instance);
	return *Instance;
}

void IRenderer::Destroy()
{
	Instance.reset();
}

NAMESPACE_END(RHI)