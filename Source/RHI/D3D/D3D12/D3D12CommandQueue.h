#pragma once

#include "RHI/RHIDevice.h"
#include "RHI/D3D/D3D12/D3D12Types.h"

class D3D12CommandQueue : public D3DHwResource<ID3D12CommandQueue>
{
public:
	D3D12CommandQueue(const class D3D12Device& Device, ERHIDeviceQueue Type);

	~D3D12CommandQueue();
protected:
private:
};
