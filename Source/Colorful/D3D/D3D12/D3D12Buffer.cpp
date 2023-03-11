#include "Colorful/D3D/D3D12/D3D12Buffer.h"
#include "Colorful/D3D/D3D12/D3D12Device.h"

NAMESPACE_START(RHI)

D3D12Buffer::D3D12Buffer(D3D12Device* Device, const BufferDesc& Desc)
{
	assert(Device);

	static const size_t ConstantsBufferAlignment = 256ull;
	
	D3D12_RESOURCE_DESC CreateDesc
	{
		D3D12_RESOURCE_DIMENSION_BUFFER,
		0u,
		Desc.Size,
		1u,
		1u,
		1u,
		DXGI_FORMAT_UNKNOWN,
		DXGI_SAMPLE_DESC
		{
			1u,
			0u
		},
		D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
		D3D12_RESOURCE_FLAG_NONE
	};

	if (EnumHasAnyFlags(Desc.UsageFlags, EBufferUsageFlags::UniformBuffer))
	{
		CreateDesc.Width = Align(CreateDesc.Width, ConstantsBufferAlignment); /// TODO ???
	}
	if (EnumHasAnyFlags(Desc.UsageFlags, EBufferUsageFlags::UnorderedAccess))
	{
		CreateDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	D3D12_HEAP_PROPERTIES HeapProperties
	{
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		0u,
		0u
	};

	D3D12_RESOURCE_STATES States = D3D12_RESOURCE_STATE_COMMON;
	if (Desc.AccessFlags == EDeviceAccessFlags::None)
	{
		assert(0);
	}
	if (EnumHasAnyFlags(Desc.AccessFlags, EDeviceAccessFlags::CpuRead))
	{
		HeapProperties.Type = D3D12_HEAP_TYPE_READBACK;
		States = D3D12_RESOURCE_STATE_COPY_DEST;
		m_HostVisible = true;
	}
	if (EnumHasAnyFlags(Desc.AccessFlags, EDeviceAccessFlags::CpuWrite) || EnumHasAnyFlags(Desc.AccessFlags, EDeviceAccessFlags::GpuReadCpuWrite))
	{
		HeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
		States = D3D12_RESOURCE_STATE_GENERIC_READ;
	}

	m_Size = CreateDesc.Width;

	/// https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_resource_states
	/// Creates both a resource and an implicit heap, 
	/// such that the heap is big enough to contain the entire resource, 
	/// and the resource is mapped to the heap.
	VERIFY_D3D(Device->Get()->CreateCommittedResource(
		&HeapProperties, 
		D3D12_HEAP_FLAG_NONE, 
		&CreateDesc, 
		States, 
		nullptr, 
		IID_PPV_ARGS(Reference())));
}

void* D3D12Buffer::Map(size_t Size, size_t Offset)
{
	assert(m_HostVisible && m_MappedMemory == nullptr && (Size == WHOLE_SIZE || Offset + Size <= m_Size));

	D3D12_RANGE Range
	{
		Offset,
		Offset + Size
	};

	VERIFY_D3D(m_Handle->Map(0u, &Range, &m_MappedMemory));

	return m_MappedMemory;
}

void D3D12Buffer::Unmap()
{
	assert(m_HostVisible && m_MappedMemory);

	m_Handle->Unmap(0u, nullptr);

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

bool8_t D3D12Buffer::Update(const void* Data, size_t Size, size_t SrcOffset, size_t DstOffset)
{
	assert(Data && Size && Size <= m_Size);

	if (m_MappedMemory)
	{
		VERIFY(memcpy_s(
			reinterpret_cast<byte8_t*>(m_MappedMemory) + DstOffset,
			Size,
			reinterpret_cast<const byte8_t*>(Data) + SrcOffset,
			Size) == 0);

		///if (!m_IsCoherent)
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

NAMESPACE_END(RHI)
