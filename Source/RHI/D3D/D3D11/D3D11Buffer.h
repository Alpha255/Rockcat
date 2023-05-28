#pragma once

#include "Colorful/D3D/DXGI_Interface.h"

NAMESPACE_START(Gfx)

class D3D11Buffer final : public D3DObject<ID3D11Buffer>, public IBuffer
{
public:
	D3D11Buffer(ID3D11Device* device, EResourceUsages usages, EDeviceAccessMode accessMode, size_t size, const void* data);

	void unmap() override final;
protected:
	void* map_native(size_t size, size_t offset) override final;
	void update_native(const void* data, size_t size, size_t offset, bool8_t persistence) override final;
	void flushMappedRange_native(size_t size, size_t offset) override final;
};

NAMESPACE_END(Gfx)
