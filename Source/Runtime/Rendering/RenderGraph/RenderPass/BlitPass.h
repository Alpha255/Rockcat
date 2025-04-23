#pragma once

#include "Rendering/RenderGraph/RenderPass.h"

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

	void SetMode(EMode Mode) { m_Mode = Mode; }

	void Compile() override final {}

	void Execute(const RenderScene&) override final {}
protected:
private:
	EMode m_Mode = EMode::FullscreenTriangle;
	ESampler m_Sampler = ESampler::Point;
};