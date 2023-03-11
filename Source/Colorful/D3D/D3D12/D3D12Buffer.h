#pragma once

#include "Colorful/D3D/D3D12/D3D12Types.h"

NAMESPACE_START(RHI)

class D3D12Buffer : public D3DHWObject<IBuffer, ID3D12Resource>
{
public:
	D3D12Buffer(class D3D12Device* Device, const BufferDesc& Desc);

	~D3D12Buffer();

	void* Map(size_t Size, size_t Offset) override final;

	void Unmap() override final;

	void FlushMappedRange(size_t Size, size_t Offset) override final;

	void InvalidateMappedRange(size_t Size, size_t Offset) override final;

	bool8_t Update(const void* Data, size_t Size, size_t SrcOffset, size_t DstOffset) override final;
protected:
private:
	size_t m_Size = 0ull;
	bool8_t m_HostVisible = false;
};

NAMESPACE_END(RHI)
