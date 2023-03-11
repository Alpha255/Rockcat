#include "Colorful/IRenderer/StagingBufferAllocator.h"
#include "Colorful/IRenderer/IDevice.h"
#include "Colorful/IRenderer/IBuffer.h"

NAMESPACE_START(RHI)

StagingBufferAllocator::StagingBufferAllocator(IDevice* RHIDevice, size_t HeapSize)
	: m_Device(RHIDevice)
	, m_HeapSize(HeapSize)
{
	assert(m_Device);
}

std::shared_ptr<StagingBufferAllocator::Heap> StagingBufferAllocator::Alloc(size_t Size, size_t Alignment)
{
	std::shared_ptr<Heap> HeapToRecycle;

	if (m_CurHeap)
	{
		size_t AlignedOffset = Align(m_CurHeap->CurEnd, Alignment);
		size_t CurEnd = AlignedOffset + Size;

		if (CurEnd <= m_CurHeap->Size)
		{
			m_CurHeap->CurEnd = CurEnd;
			m_CurHeap->CurOffset = AlignedOffset;

			return m_CurHeap;
		}

		HeapToRecycle = m_CurHeap;
		m_CurHeap.reset();
	}

	for (auto HeapIt = m_Heaps.begin(); HeapIt != m_Heaps.end(); ++HeapIt)
	{
		auto Heap = *HeapIt;
		if (Heap->Size >= Size)
		{
			m_Heaps.erase(HeapIt);
			m_CurHeap = Heap;
			break;
		}
	}

	if (HeapToRecycle)
	{
		m_Heaps.emplace_back(std::move(HeapToRecycle));
	}

	if (!m_CurHeap)
	{
		m_CurHeap = AllocHeap(Align(std::max(Size, m_HeapSize), DefaultHeapSize));
	}

	m_CurHeap->CurEnd = Size;

	return m_CurHeap;
}

std::shared_ptr<StagingBufferAllocator::Heap> StagingBufferAllocator::AllocHeap(size_t Size)
{
	assert(Size);

	auto TempHeap = std::make_shared<Heap>();
	TempHeap->Size = Size;

	BufferDesc Desc
	{
		EBufferUsageFlags::None,
		EDeviceAccessFlags::GpuReadCpuWrite,
		EResourceState::Common,
		Size
	};

	TempHeap->Buffer = m_Device->CreateBuffer(Desc);

	return TempHeap;
}

void StagingBufferAllocator::Submit()
{
	if (m_CurHeap)
	{
		m_Heaps.emplace_back(std::move(m_CurHeap));
		m_CurHeap.reset();
	}
}

NAMESPACE_END(RHI)