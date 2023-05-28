#include "Colorful/D3D/D3D11/D3D11Command.h"
#include "Colorful/D3D/D3D11/D3D11RenderState.h"

NAMESPACE_START(Gfx)

static D3D11RenderState s_RenderState;

D3D11CommandBuffer::D3D11CommandBuffer(ID3D11Device* device)
	: m_Type(EType::Deferred)
{
	assert(device);
	VERIFY_D3D(device->CreateDeferredContext(0u, &m_Context));
}

NAMESPACE_END(Gfx)