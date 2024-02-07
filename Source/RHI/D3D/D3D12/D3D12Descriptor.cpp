#include "RHI/D3D/D3D12/D3D12Descriptor.h"
#include "RHI/D3D/D3D12/D3D12Device.h"
#include "Engine/Services/SpdLogService.h"

static constexpr uint32_t DescriptorHeapLimits[] =
{
	1024u, DESCRIPTION(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
	128u,  DESCRIPTION(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
	256u,  DESCRIPTION(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
	256u,  DESCRIPTION(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
};

D3D12DescriptorHeap::D3D12DescriptorHeap(const D3D12Device& Device, const D3D12_DESCRIPTOR_HEAP_DESC& DescriptorHeapDesc, const uint32_t DescriptorSize)
	: m_DescriptorSize(DescriptorSize)
{
	assert(DescriptorHeapDesc.Type < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES);

	/******************************************************************************************************
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		The flag D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE can optionally be set on a descriptor heap to indicate it is be bound on a command list for reference by shaders. 
		Descriptor heaps created without this flag allow applications the option to stage descriptors in CPU memory before copying them to a shader visible descriptor heap, 
		as a convenience. But it is also fine for applications to directly create descriptors into shader visible descriptor heaps with no requirement to stage anything on the CPU.
	*******************************************************************************************************/

	VERIFY_D3D(Device->CreateDescriptorHeap(&DescriptorHeapDesc, IID_PPV_ARGS(Reference())));

	m_CpuHandleStart = GetNative()->GetCPUDescriptorHandleForHeapStart();
	if (DescriptorHeapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
	{
		m_GpuHandleStart = GetNative()->GetGPUDescriptorHandleForHeapStart();
	}

	m_FreeRanges.push_back(FreeRange{ m_CpuHandleStart.ptr, m_CpuHandleStart.ptr + DescriptorHeapDesc.NumDescriptors * m_DescriptorSize });
}

D3D12DescriptorHeap::HeapOffset D3D12DescriptorHeap::Allocate()
{
	auto& Range = m_FreeRanges.front();

	HeapOffset Ret = Range.Start;
	Range.Start += m_DescriptorSize;

	if (Range.Start == Range.End)
	{
		m_FreeRanges.pop_front();
	}

	return Ret;
}

void D3D12DescriptorHeap::Free(const D3D12_CPU_DESCRIPTOR_HANDLE& CpuDescriptorHandle)
{
	for (auto FreeRangeIt = m_FreeRanges.begin(); FreeRangeIt != m_FreeRanges.end(); ++FreeRangeIt)
	{
		assert(FreeRangeIt->Start < FreeRangeIt->End);

		if (FreeRangeIt->Start == CpuDescriptorHandle.ptr + m_DescriptorSize)
		{
			FreeRangeIt->Start = CpuDescriptorHandle.ptr;
			break;
		}
		else if (FreeRangeIt->End == CpuDescriptorHandle.ptr)
		{
			FreeRangeIt->End += m_DescriptorSize;
			break;
		}
		else
		{
			assert(FreeRangeIt->End < CpuDescriptorHandle.ptr || FreeRangeIt->Start > CpuDescriptorHandle.ptr);
			if (FreeRangeIt->Start > CpuDescriptorHandle.ptr)
			{
				m_FreeRanges.push_back(FreeRange{ CpuDescriptorHandle.ptr, CpuDescriptorHandle.ptr + m_DescriptorSize});
				break;
			}
		}
	}

	assert(false);
}

D3D12DescriptorAllocator::D3D12DescriptorAllocator(const D3D12Device& Device, D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeapType)
	: m_Device(Device)
{
	m_DescriptorSize = Device->GetDescriptorHandleIncrementSize(DescriptorHeapType);

	m_Description = D3D12_DESCRIPTOR_HEAP_DESC
	{
		DescriptorHeapType,
		DescriptorHeapLimits[DescriptorHeapType],
		DescriptorHeapType > D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER ? D3D12_DESCRIPTOR_HEAP_FLAG_NONE : D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		0u
	};
}

D3D12DescriptorHandle D3D12DescriptorAllocator::Allocate()
{
	if (m_FreeHeaps.empty())
	{
		m_Heaps.push_back(std::make_unique<D3D12DescriptorHeap>(m_Device, m_Description, m_DescriptorSize));
		m_FreeHeaps.push_back(static_cast<D3D12DescriptorHeap::HeapIndex>(m_Heaps.size() - 1u));
	}
	assert(!m_FreeHeaps.empty());

	D3D12DescriptorHandle Ret
	{
		m_FreeHeaps.front()
	};

	auto Heap = m_Heaps[Ret.Index].get();
	assert(Heap && !Heap->IsFull());
	Ret.Handle.ptr = Heap->Allocate();

	if (Heap->IsFull())
	{
		m_FreeHeaps.pop_front();
	}

	return Ret;
}

void D3D12DescriptorAllocator::Free(const D3D12DescriptorHandle& DescriptorHandle)
{
	assert(DescriptorHandle.Index < m_Heaps.size());
	m_Heaps[DescriptorHandle.Index]->Free(DescriptorHandle.Handle);
}
