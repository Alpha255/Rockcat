#pragma once

#include "RHI/D3D/D3D12/D3D12Types.h"

class D3D12Buffer : public D3DHwResource<ID3D12Resource>, public RHIBuffer
{
public:
	D3D12Buffer(const class D3D12Device& Device, const RHIBufferDesc& Desc);

	~D3D12Buffer();

	void* Map(size_t Size, size_t Offset) override final;

	void Unmap() override final;

	void FlushMappedRange(size_t Size, size_t Offset) override final;

	void InvalidateMappedRange(size_t Size, size_t Offset) override final;

	bool Update(const void* Data, size_t Size, size_t SrcOffset, size_t DstOffset) override final;

	D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return m_GpuVirtualAddress; }
protected:
private:
	size_t m_Size = 0ull;
	bool m_HostVisible = false;
	D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress{};
};
