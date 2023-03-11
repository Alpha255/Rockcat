#include "Colorful/D3D/D3D12/D3D12CommandQueue.h"
#include "Colorful/D3D/D3D12/D3D12Device.h"

NAMESPACE_START(RHI)

D3D12CommandQueue::D3D12CommandQueue(ID3D12Device* Device, EQueueType Type)
{
	assert(Device);

	D3D12_COMMAND_QUEUE_DESC Desc
	{
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
		D3D12_COMMAND_QUEUE_FLAG_NONE,
		0u
	};

	std::wstring_view Name;
	switch (Type)
	{
	case EQueueType::Graphics:
		Desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		Name = L"D3D12 Graphics Queue";
		break;
	case EQueueType::Transfer:
		Desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		Name = L"D3D12 Transfer Queue";
		break;
	case EQueueType::Compute:
		Desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		Name = L"D3D12 Compute Queue";
		break;
	}

	VERIFY_D3D(Device->CreateCommandQueue(&Desc, IID_PPV_ARGS(Reference())));

	Get()->SetName(Name.data());
}

D3D12CommandQueue::~D3D12CommandQueue()
{
}

NAMESPACE_END(RHI)
