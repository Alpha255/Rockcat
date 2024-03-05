#include "RHI/D3D/D3D12/D3D12CommandQueue.h"
#include "RHI/D3D/D3D12/D3D12Device.h"
#include "Engine/Services/SpdLogService.h"

D3D12CommandQueue::D3D12CommandQueue(const D3D12Device& Device, ERHIDeviceQueue Type)
{
	D3D12_COMMAND_QUEUE_DESC Desc
	{
		.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
		.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
		.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
		.NodeMask = 0u
	};

	std::string_view Name;
	switch (Type)
	{
	case ERHIDeviceQueue::Graphics:
		Desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		Name = "D3D12 Graphics Queue";
		break;
	case ERHIDeviceQueue::Transfer:
		Desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		Name = "D3D12 Transfer Queue";
		break;
	case ERHIDeviceQueue::Compute:
		Desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		Name = "D3D12 Compute Queue";
		break;
	}

	VERIFY_D3D(Device->CreateCommandQueue(&Desc, IID_PPV_ARGS(Reference())));

	SetDebugName(Name.data());
}

D3D12CommandQueue::~D3D12CommandQueue()
{
}
