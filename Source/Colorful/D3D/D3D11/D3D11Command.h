#pragma once

#include "Colorful/D3D/D3D11/D3D11Device.h"

NAMESPACE_START(Gfx)

class D3D11CommandBuffer : public ICommandBuffer
{
public:
	D3D11CommandBuffer(ID3D11DeviceContext* context)
		: m_Context(context)
	{
		assert(m_Context);
	}

	D3D11CommandBuffer(ID3D11Device* device);
protected:
	enum class EType
	{
		Immediate,
		Deferred
	};

	ID3D11DeviceContext* m_Context = nullptr;
	EType m_Type = EType::Immediate;
private:
};

class D3D11CommandList : public D3DObject<ID3D11CommandList>, public D3D11CommandBuffer
{
public:
	using D3D11CommandBuffer::D3D11CommandBuffer;
};

NAMESPACE_END(Gfx)
