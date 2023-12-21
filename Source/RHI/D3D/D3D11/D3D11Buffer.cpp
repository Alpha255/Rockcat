#include "RHI/D3D/D3D11/D3D11Buffer.h"
#include "RHI/D3D/D3D11/D3D11Device.h"
#include "Runtime/Engine/Services/SpdLogService.h"

D3D11Buffer::D3D11Buffer(const D3D11Device& Device, const RHIBufferCreateInfo& RHICreateInfo)
{
	static const size_t ConstantsBufferAlignment = 256ull;
	
	D3D11_RESOURCE_DESC CreateDesc
	{
		D3D11_RESOURCE_DIMENSION_BUFFER,
		0u,
		RHICreateInfo.Size,
		1u,
		1u,
		1u,
		DXGI_FORMAT_UNKNOWN,
		DXGI_SAMPLE_DESC
		{
			1u,
			0u
		},
		D3D11_TEXTURE_LAYOUT_ROW_MAJOR,
		D3D11_RESOURCE_FLAG_NONE
	};

	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::UniformBuffer))
	{
		CreateDesc.Width = Align(CreateDesc.Width, ConstantsBufferAlignment); /// TODO ???
	}
	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::UnorderedAccess))
	{
		CreateDesc.Flags |= D3D11_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	D3D11_HEAP_PROPERTIES HeapProperties
	{
		D3D11_HEAP_TYPE_DEFAULT,
		D3D11_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D11_MEMORY_POOL_UNKNOWN,
		0u,
		0u
	};

	D3D11_RESOURCE_STATES States = D3D11_RESOURCE_STATE_COMMON;
	if (RHICreateInfo.AccessFlags == ERHIDeviceAccessFlags::None)
	{
		assert(0);
	}
	if (EnumHasAnyFlags(RHICreateInfo.AccessFlags, ERHIDeviceAccessFlags::CpuRead))
	{
		HeapProperties.Type = D3D11_HEAP_TYPE_READBACK;
		States = D3D11_RESOURCE_STATE_COPY_DEST;
		m_HostVisible = true;
	}
	if (EnumHasAnyFlags(RHICreateInfo.AccessFlags, ERHIDeviceAccessFlags::CpuWrite) || EnumHasAnyFlags(RHICreateInfo.AccessFlags, ERHIDeviceAccessFlags::GpuReadCpuWrite))
	{
		HeapProperties.Type = D3D11_HEAP_TYPE_UPLOAD;
		States = D3D11_RESOURCE_STATE_GENERIC_READ;
	}

	m_Size = CreateDesc.Width;

	/// https://docs.microsoft.com/en-us/windows/win32/api/D3D11/ne-D3D11-D3D11_resource_states
	/// Creates both a resource and an implicit heap, 
	/// such that the heap is big enough to contain the entire resource, 
	/// and the resource is mapped to the heap.
	VERIFY_D3D(Device->CreateCommittedResource(
		&HeapProperties, 
		D3D11_HEAP_FLAG_NONE, 
		&CreateDesc, 
		States, 
		nullptr, 
		IID_PPV_ARGS(Reference())));
}

void* D3D11Buffer::Map(size_t Size, size_t Offset)
{
	assert(m_HostVisible && m_MappedMemory == nullptr && (Size == WHOLE_SIZE || Offset + Size <= m_Size));

	D3D11_RANGE Range
	{
		Offset,
		Offset + Size
	};

	VERIFY_D3D(GetNative()->Map(0u, &Range, &m_MappedMemory));

	return m_MappedMemory;
}

void D3D11Buffer::Unmap()
{
	assert(m_HostVisible && m_MappedMemory);

	GetNative()->Unmap(0u, nullptr);

	m_MappedMemory = nullptr;
}

void D3D11Buffer::FlushMappedRange(size_t Size, size_t Offset)
{
	(void)Size;
	(void)Offset;
	assert(false);
}

void D3D11Buffer::InvalidateMappedRange(size_t Size, size_t Offset)
{
	(void)Size;
	(void)Offset;
	assert(false);
}

bool8_t D3D11Buffer::Update(const void* Data, size_t Size, size_t SrcOffset, size_t DstOffset)
{
	assert(Data && Size && Size <= m_Size);

	if (m_MappedMemory)
	{
		VERIFY(memcpy_s(
			reinterpret_cast<byte8_t*>(m_MappedMemory) + DstOffset,
			Size,
			reinterpret_cast<const byte8_t*>(Data) + SrcOffset,
			Size) == 0);

		if (!m_HostVisible)
		{
			FlushMappedRange(Size, DstOffset);
		}

		return true;
	}

	return false;
}

D3D11Buffer::~D3D11Buffer()
{
	if (m_MappedMemory)
	{
		Unmap();
	}
}
