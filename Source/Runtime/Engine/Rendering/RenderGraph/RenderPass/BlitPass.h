#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass.h"

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

	void Compile() override final {}

	void Execute(class RHIDevice&, const RenderScene&) override final {}
protected:
private:
	EMode m_Mode = EMode::FullscreenTriangle;
	ESampler m_Sampler = ESampler::Point;
};