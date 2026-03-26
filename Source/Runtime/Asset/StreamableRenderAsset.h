#pragma once

#include "Core/Definitions.h"

class StreamableRenderAsset
{
public:
	virtual ~StreamableRenderAsset();

	virtual bool StreamIn(uint32_t /*NumMips*/) { return false; }
	virtual bool StreamOut(uint32_t /*NumMips*/) { return false; }
};
