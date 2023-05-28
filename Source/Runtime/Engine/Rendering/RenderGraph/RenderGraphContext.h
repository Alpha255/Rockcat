#pragma once

#include "Colorful/IRenderer/IResource.h"

NAMESPACE_START(RHI)

class RenderGraphContext
{
public:
	RenderGraphContext(IDevice* RHIDevice, class Scene* TargetScene)
		: m_RenderDevice(RHIDevice)
		, m_Scene(TargetScene)
	{
		assert(m_RenderDevice && m_Scene);
	}

	void SetSwapchainFrameBuffer(IFrameBuffer* FrameBuffer)
	{
		m_SwapchainFrameBuffer = FrameBuffer;
	}

	IFrameBuffer* SwapchinFrameBuffer() const
	{
		return m_SwapchainFrameBuffer;
	}

	class Scene* TargetScene() const
	{
		return m_Scene;
	}

	ISamplerSharedPtr GetOrCreateSampler(const struct SamplerDesc& Desc);

	IPipelineSharedPtr GetOrCreateGraphicsPipeline(const struct GraphicsPipelineDesc& Desc);
protected:
private:
	class Scene* m_Scene = nullptr;
	IDevice* m_RenderDevice = nullptr;
	IFrameBuffer* m_SwapchainFrameBuffer = nullptr;
	std::unordered_map<size_t, ISamplerSharedPtr> m_Samplers;
	std::unordered_map<size_t, IPipelineSharedPtr> m_GraphicsPipelines;
};

NAMESPACE_END(RHI)
