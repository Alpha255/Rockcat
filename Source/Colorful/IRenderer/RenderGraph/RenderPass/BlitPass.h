#pragma once

#include "Colorful/IRenderer/RenderGraph/IFrameGraph.h"

NAMESPACE_START(RHI)

class BlitPass : public IFrameGraphPass
{
public:
	enum class EMode
	{
		CopyImage,
		FullscreenTriangle,
		FullscreenQuad,
		GpuGenFullscreenTriangle,
		GpuGenFullscreenQuad
	};

	enum class ESampler
	{
		Point,
		Linear
	};

	BlitPass(const Scene* TargetScene, const Camera* ViewCamera);

	void SetSource(IImageSharedPtr Src)
	{
		m_BlitSrc = Src;
	}

	void ApplyRenderSettings(const RenderSettings* Settings) override final;

	void Render(ICommandBuffer* Command) override final;

	void SetupMaterial(const MeshInstance*) override final;

	void SetMode(EMode Mode);
protected:
private:
	EMode m_Mode = EMode::FullscreenTriangle;
	ESampler m_Sampler = ESampler::Point;
	Drawable m_Drawable;
	std::unordered_map<size_t, IPipelineSharedPtr> m_GraphicsPipelines;
	std::shared_ptr<MeshInstance> m_FullscreenTriangle;
	std::shared_ptr<MeshInstance> m_FullscreenQuad;
	IImageSharedPtr m_BlitSrc;
	ISamplerSharedPtr m_LinearSampler;
	ISamplerSharedPtr m_PointSampler;
	static ImageSlice AllSlice;
};

NAMESPACE_END(RHI)