#pragma once

#include "RHI/D3D/D3D12/D3D12Types.h"

class D3D12Fence : public D3DHwResource<ID3D12Fence>
{
public:
	D3D12Fence(const class D3D12Device& Device);
protected:
private:
};

#if 0
struct D3D12ResourceBarrier
{
	D3D12ResourceBarrier(class D3D12CommandList* CommandList)
		: Command(CommandList)
	{
		assert(CommandList);
	}

	D3D12ResourceBarrier& Transition(class D3D12Image* Image, EResourceState Src, EResourceState Dst, const ImageSubresourceRange& SubresourceRange);
	D3D12ResourceBarrier& Transition(class D3D12Buffer* Buffer, EResourceState Src, EResourceState Dst);

	void Commit();
protected:
	std::vector<D3D12_RESOURCE_BARRIER> Barriers;
	class D3D12CommandList* Command = nullptr;
};
#endif
