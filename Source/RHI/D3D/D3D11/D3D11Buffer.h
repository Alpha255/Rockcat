#pragma once

#include "RHI/D3D/D3D11/D3D11Types.h"
#include "Engine/RHI/RHIBuffer.h"

class D3D11Buffer final : public D3DHwResource<ID3D11Buffer>, public RHIBuffer
{
public:
	D3D11Buffer(const class D3D11Device& Device, const RHIBufferCreateInfo& RHICreateInfo);

	~D3D11Buffer();

	void* Map(size_t Size, size_t Offset) override final;

	void Unmap() override final;

	void FlushMappedRange(size_t Size, size_t Offset) override final;

	virtual void InvalidateMappedRange(size_t Size, size_t Offset) override final;

	virtual bool Update(const void* Data, size_t Size, size_t SrcOffset, size_t DstOffset) override final;
protected:
};
