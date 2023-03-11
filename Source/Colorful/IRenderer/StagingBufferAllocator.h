#pragma once

#include "Colorful/IRenderer/Declarations.h"

NAMESPACE_START(RHI)

/// For temp
class StagingBufferAllocator : public LazySingleton<StagingBufferAllocator>
{
public:
	struct Heap
	{
		size_t Size = 0u;
		size_t CurOffset = 0u;
		size_t CurEnd = 0u;
		IBufferPtr Buffer;
	};

	std::shared_ptr<Heap> Alloc(size_t Size, size_t Alignment = Kilobyte / 4ull);

	void Submit();
protected:
	ALLOW_ACCESS_LAZY(StagingBufferAllocator);

	StagingBufferAllocator(class IDevice* RHIDevice, size_t HeapSize = DefaultHeapSize);

	std::shared_ptr<Heap> AllocHeap(size_t Size);
private:
	class IDevice* m_Device = nullptr;
	size_t m_HeapSize = ~0ull;
	std::shared_ptr<Heap> m_CurHeap;
	std::list<std::shared_ptr<Heap>> m_Heaps;

	std::mutex m_Mutex; /// #TODO: Support multi thread

	static constexpr size_t DefaultHeapSize = Kilobyte * 64ull;
	static constexpr size_t DefaultAlignment = Kilobyte * 4ull;
};

NAMESPACE_END(RHI)