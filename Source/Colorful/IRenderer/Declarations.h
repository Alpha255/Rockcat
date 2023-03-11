#pragma once

#include "Core/Gear.h"

NAMESPACE_START(RHI)

DECLARE_SHARED_PTR(IBuffer)
DECLARE_SHARED_PTR(IShader)
DECLARE_SHARED_PTR(IImage)
DECLARE_SHARED_PTR(ISampler)
DECLARE_SHARED_PTR(ICommandBuffer)
DECLARE_SHARED_PTR(ICommandBufferPool)
DECLARE_SHARED_PTR(IInputLayout)
DECLARE_SHARED_PTR(IPipeline)
DECLARE_SHARED_PTR(IFrameBuffer)
DECLARE_UNIQUE_PTR(IDevice)
DECLARE_UNIQUE_PTR(IRenderer)

NAMESPACE_END(RHI)


