#pragma once

#include "RHI/D3D/D3D11/D3D11Types.h"
#include "RHI/RHICommandBuffer.h"

class D3D11CommandList : public D3DHwResource<ID3D11CommandList>, public RHICommandBuffer
{
};
