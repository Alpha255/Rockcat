#pragma once

#include "Rendering/RenderGraph/RenderPassParameters.h"

class RDGRenderTargetPool
{
public:
	void GetStats(uint32_t& NumRenderTargets, uint32_t& MemorySizeOfPool, uint32_t& MemorySizeUsed);


private:
	RDGRenderTarget* Create(RHITextureDesc& Desc, const char* Name);
};

