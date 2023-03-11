#include "Colorful/D3D/D3D11/D3D11Buffer.h"
#include "Colorful/D3D/D3D11/D3D11EnumTranslator.h"

NAMESPACE_START(Gfx)

D3D11Buffer::D3D11Buffer(ID3D11Device* device, EResourceUsages usages, EDeviceAccessMode accessMode, size_t size, const void* data)
{
	/// https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-subresources

	assert(device);

	///m_Size = size;

	D3D11_BUFFER_DESC desc
	{
		static_cast<uint32_t>(size),
		D3D11_USAGE_DEFAULT,
		0u,
		0u,
		0u,
		0u
	};

	if (!!(accessMode & EDeviceAccessMode::GpuRead))
	{
		desc.Usage = D3D11_USAGE_IMMUTABLE;
	}
	else if (!!(accessMode & EDeviceAccessMode::GpuReadWrite))
	{
		desc.Usage = D3D11_USAGE_DEFAULT;
	}
	else if (!!(accessMode & EDeviceAccessMode::GpuReadCpuWrite))
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
	}
	else if (!!(accessMode & EDeviceAccessMode::Staging))
	{
		desc.Usage = D3D11_USAGE_STAGING;
	}

	if (!!(accessMode & EDeviceAccessMode::CpuRead))
	{
		desc.CPUAccessFlags |= D3D11_CPU_ACCESS_READ;
	}
	if (!!(accessMode & EDeviceAccessMode::CpuWrite))
	{
		desc.CPUAccessFlags |= D3D11_CPU_ACCESS_WRITE;
	}

	if (!!(usages & EResourceUsages::VertexBuffer))
	{
		desc.BindFlags |= D3D11_BIND_VERTEX_BUFFER;
	}
	else if (!!(usages & EResourceUsages::IndexBuffer))
	{
		desc.BindFlags |= D3D11_BIND_INDEX_BUFFER;
	}
	else if (!!(usages & EResourceUsages::UniformBuffer))
	{
		desc.BindFlags |= D3D11_BIND_CONSTANT_BUFFER;
	}
	else if (!!(usages & EResourceUsages::UnorderedAccess))
	{
		desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}
	
	if (!!(usages & EResourceUsages::ShaderResource))
	{
		desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}
	if (!!(usages & EResourceUsages::IndirectBuffer))
	{
		desc.MiscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	}
	if (!!(usages & EResourceUsages::RawBuffer))
	{
		desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	}
	if (!!(usages & EResourceUsages::StructuredBuffer))
	{
		desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	}

	D3D11_SUBRESOURCE_DATA subresource
	{
		data,
		size,
		0u
	};

	VERIFY_D3D(device->CreateBuffer(&desc, ((nullptr == data) ? nullptr : &subresource), reference()));
}

void* D3D11Buffer::map_native(size_t size, size_t offset)
{
	assert(isValid() && offset < m_Size && (~size == 0ULL || offset + size <= m_Size));

	if (!m_MappedMemory.Memory)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;
		VERIFY_D3D(s_IMContext->Map(get(), 0u, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0u, &mappedSubresource));
		m_MappedMemory.Memory = mappedSubresource.pData;
		m_MappedMemory.Size = size;
		m_MappedMemory.Offset = offset;
	}

	return m_MappedMemory.Memory;
}

void D3D11Buffer::update_native(const void* data, size_t size, size_t offset, bool8_t persistence)
{
	assert(isValid() && offset < m_Size && (~size == 0ULL || offset + size <= m_Size));

	map(size, offset);
	VERIFY(memcpy_s(m_MappedMemory.Memory, size, data, size) == 0);
	if (persistence)
	{
		flushMappedRange(size, offset);
	}
	else
	{
		unmap();
	}
}

void D3D11Buffer::flushMappedRange_native(size_t size, size_t offset)
{
	assert(0);
	(void)size;
	(void)offset;
}

void D3D11Buffer::unmap()
{
	assert(isValid());

	if (m_MappedMemory.Memory)
	{
		s_IMContext->Unmap(get(), 0u);
		m_MappedMemory.Memory = nullptr;
		m_MappedMemory.Size = 0u;
		m_MappedMemory.Offset = 0u;
	}
}

NAMESPACE_END(Gfx)