#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPass.h"

class BlitPass : public RenderPass
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

	DECLARE_RENDERPASS_CONSTRUCTOR(BlitPass, RenderPass)

	void SetMode(EMode Mode) { m_Mode = Mode; }

	void Execute(class RHIDevice&, const class Scene&) override final {}
protected:
private:
	EMode m_Mode = EMode::FullscreenTriangle;
	ESampler m_Sampler = ESampler::Point;
};