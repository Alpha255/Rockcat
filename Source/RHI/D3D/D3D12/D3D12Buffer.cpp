#include "RHI/D3D/D3D12/D3D12Buffer.h"
#include "RHI/D3D/D3D12/D3D12Device.h"
#include "Engine/Services/SpdLogService.h"

D3D12Buffer::D3D12Buffer(const D3D12Device& Device, const RHIBufferCreateInfo& RHICreateInfo)
{
	assert(RHICreateInfo.AccessFlags != ERHIDeviceAccessFlags::None);

	static const size_t ConstantsBufferAlignment = 256ull;
	
	DXGI_SAMPLE_DESC SampleDesc
	{
		.Count = 1u,
		.Quality = 0u
	};

	D3D12_RESOURCE_DESC CreateDesc
	{
		.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
		.Alignment = 0u,
		.Width = RHICreateInfo.Size,
		.Height = 1u,
		.DepthOrArraySize = 1u,
		.MipLevels = 1u,
		.Format = DXGI_FORMAT_UNKNOWN,
		.SampleDesc = SampleDesc,
		.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
		.Flags = D3D12_RESOURCE_FLAG_NONE
	};

	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::UniformBuffer))
	{
		CreateDesc.Width = Align(CreateDesc.Width, ConstantsBufferAlignment); /// TODO ???
	}
	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::UnorderedAccess))
	{
		CreateDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	D3D12_HEAP_PROPERTIES HeapProperties
	{
		.Type = D3D12_HEAP_TYPE_DEFAULT,
		.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
		.CreationNodeMask = 0u,
		.VisibleNodeMask = 0u
	};

	D3D12_RESOURCE_STATES States = D3D12_RESOURCE_STATE_COMMON;

	if (EnumHasAnyFlags(RHICreateInfo.AccessFlags, ERHIDeviceAccessFlags::CpuRead))
	{
		HeapProperties.Type = D3D12_HEAP_TYPE_READBACK;
		States = D3D12_RESOURCE_STATE_COPY_DEST;
		m_HostVisible = true;
	}
	if (EnumHasAnyFlags(RHICreateInfo.AccessFlags, ERHIDeviceAccessFlags::CpuWrite) || EnumHasAnyFlags(RHICreateInfo.AccessFlags, ERHIDeviceAccessFlags::GpuReadCpuWrite))
	{
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		States = D3D12_RESOURCE_STATE_GENERIC_READ;
	}

	m_Size = CreateDesc.Width;

	/// https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_resource_states
	/// Creates both a resource and an implicit heap, 
	/// such that the heap is big enough to contain the entire resource, 
	/// and the resource is mapped to the heap.
	VERIFY_D3D(Device->CreateCommittedResource(
		&HeapProperties, 
		D3D12_HEAP_FLAG_NONE, 
		&CreateDesc, 
		States, 
		nullptr, 
		IID_PPV_ARGS(Reference())));

	m_GpuVirtualAddress = GetNative()->GetGPUVirtualAddress();
}

void* D3D12Buffer::Map(size_t Size, size_t Offset)
{
	assert(m_HostVisible && m_MappedMemory == nullptr && (Size == WHOLE_SIZE || Offset + Size <= m_Size));

	D3D12_RANGE Range
	{
		.Begin = Offset,
		.End = Offset + Size
	};

	VERIFY_D3D(GetNative()->Map(0u, &Range, &m_MappedMemory));

	return m_MappedMemory;
}

void D3D12Buffer::Unmap()
{
	assert(m_HostVisible && m_MappedMemory);

	GetNative()->Unmap(0u, nullptr);

	m_MappedMemory = nullptr;
}

void D3D12Buffer::FlushMappedRange(size_t Size, size_t Offset)
{
	(void)Size;
	(void)Offset;
	assert(false);
}

void D3D12Buffer::InvalidateMappedRange(size_t Size, size_t Offset)
{
	(void)Size;
	(void)Offset;
	assert(false);
}

bool D3D12Buffer::Update(const void* Data, size_t Size, size_t SrcOffset, size_t DstOffset)
{
	assert(Data && Size && Size <= m_Size);

	if (m_MappedMemory)
	{
		VERIFY(memcpy_s(
			reinterpret_cast<uint8_t*>(m_MappedMemory) + DstOffset,
			Size,
			reinterpret_cast<const uint8_t*>(Data) + SrcOffset,
			Size) == 0);

		if (!m_HostVisible)
		{
			FlushMappedRange(Size, DstOffset);
		}

		return true;
	}

	return false;
}

D3D12Buffer::~D3D12Buffer()
{
	if (m_MappedMemory)
	{
		Unmap();
	}
}
