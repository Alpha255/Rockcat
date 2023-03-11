#pragma once

#include "Colorful/IRenderer/IDevice.h"
#include "Colorful/D3D/D3D12/D3D12Types.h"

NAMESPACE_START(RHI)

class D3D12CommandQueue : public D3DHWObject<void, ID3D12CommandQueue>
{
public:
	D3D12CommandQueue(ID3D12Device* Device, EQueueType Type);

	~D3D12CommandQueue();
protected:
private:
};

NAMESPACE_END(RHI)
