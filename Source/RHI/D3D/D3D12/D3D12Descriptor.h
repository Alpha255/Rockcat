#pragma once

#include "RHI/D3D/D3D12/D3D12Types.h"

class D3D12DescriptorHeap : public D3DHwResource<ID3D12DescriptorHeap>
{
public:
	using HeapIndex = uint32_t;
	using HeapOffset = decltype(D3D12_CPU_DESCRIPTOR_HANDLE::ptr);

	D3D12DescriptorHeap(const class D3D12Device& Device, const D3D12_DESCRIPTOR_HEAP_DESC& DescriptorHeapDesc, const uint32_t DescriptorSize);

	HeapOffset Allocate();

	void Free(const D3D12_CPU_DESCRIPTOR_HANDLE& CpuDescriptorHandle);

	bool8_t IsFull() const
	{
		return m_FreeRanges.size() > 0u;
	}
protected:
	struct FreeRange
	{
		HeapOffset Start{};
		HeapOffset End{};
	};
private:
	D3D12_CPU_DESCRIPTOR_HANDLE m_CpuHandleStart{};
	D3D12_GPU_DESCRIPTOR_HANDLE m_GpuHandleStart{};
	const uint32_t m_DescriptorSize = 0u;
	std::deque<FreeRange> m_FreeRanges;
};

struct D3D12DescriptorHandle
{
	D3D12DescriptorHeap::HeapIndex Index;
	D3D12_CPU_DESCRIPTOR_HANDLE Handle;
};

class D3D12DescriptorAllocator
{
public:
	D3D12DescriptorAllocator(const class D3D12Device& Device, D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType);

	D3D12DescriptorHandle Allocate();

	void Free(const D3D12DescriptorHandle& DescriptorHandle);
protected:
private:
	const class D3D12Device& m_Device;

	D3D12_DESCRIPTOR_HEAP_DESC m_Description{};
	uint32_t m_DescriptorSize = 0u;

	std::vector<std::unique_ptr<D3D12DescriptorHeap>> m_Heaps;
	std::deque<D3D12DescriptorHeap::HeapIndex> m_FreeHeaps;
};
