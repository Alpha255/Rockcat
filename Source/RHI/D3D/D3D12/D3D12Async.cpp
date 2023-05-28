#include "Colorful/D3D/D3D12/D3D12Async.h"
#include "Colorful/D3D/D3D12/D3D12Device.h"
#include "Colorful/D3D/D3D12/D3D12CommandList.h"

NAMESPACE_START(RHI)

D3D12Fence::D3D12Fence(D3D12Device* Device)
{
	assert(Device);
	VERIFY_D3D(Device->Get()->CreateFence(0u, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(Reference())));
}

D3D12ResourceBarrier& D3D12ResourceBarrier::Transition(D3D12Image* Image, EResourceState Src, EResourceState Dst, const ImageSubresourceRange& SubresourceRange)
{
	assert(Image);

	if (Src != Dst)
	{
		D3D12_RESOURCE_BARRIER Barrier
		{
			D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		};

		Barrier.Transition.pResource = Image->Get();
		Barrier.Transition.StateBefore = D3D12Type::ResourceStates(Src);
		Barrier.Transition.StateAfter = D3D12Type::ResourceStates(Dst);

		assert(Barrier.Transition.StateBefore != Barrier.Transition.StateAfter);

		if (SubresourceRange == AllSubresource)
		{
			Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		}
		else
		{
			Barrier.Transition.Subresource = CalcSubresource(
				SubresourceRange.LevelCount, 
				SubresourceRange.LayerCount, 
				1u, 
				Image->MipLevels(),
				Image->ArrayLayers());
		}

		Barriers.emplace_back(std::move(Barrier));
	}
	else if (EnumHasAnyFlags(Dst, EResourceState::UnorderedAccess))
	{
		D3D12_RESOURCE_BARRIER Barrier
		{
			D3D12_RESOURCE_BARRIER_TYPE_UAV,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		};

		Barrier.UAV.pResource = Image->Get();

		Barriers.emplace_back(std::move(Barrier));
	}

	return *this;
}

D3D12ResourceBarrier& D3D12ResourceBarrier::Transition(D3D12Buffer* Buffer, EResourceState Src, EResourceState Dst)
{
	assert(Buffer);

	if (Src != Dst)
	{
		D3D12_RESOURCE_BARRIER Barrier
		{
			D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		};

		Barrier.Transition.pResource = Buffer->Get();
		Barrier.Transition.StateBefore = D3D12Type::ResourceStates(Src);
		Barrier.Transition.StateAfter = D3D12Type::ResourceStates(Dst);
		Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		assert(Barrier.Transition.StateBefore != Barrier.Transition.StateAfter);

		Barriers.emplace_back(std::move(Barrier));
	}
	else if (EnumHasAnyFlags(Dst, EResourceState::UnorderedAccess))
	{
		D3D12_RESOURCE_BARRIER Barrier
		{
			D3D12_RESOURCE_BARRIER_TYPE_UAV,
			D3D12_RESOURCE_BARRIER_FLAG_NONE
		};

		Barrier.UAV.pResource = Buffer->Get();

		Barriers.emplace_back(std::move(Barrier));
	}

	return *this;
}

void D3D12ResourceBarrier::Commit()
{
	if (!Barriers.empty())
	{
		Command->Get()->ResourceBarrier(static_cast<uint32_t>(Barriers.size()), Barriers.data());

		Barriers.clear();
	}
}

NAMESPACE_END(RHI)
