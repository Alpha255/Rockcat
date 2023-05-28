#pragma once

#include "Colorful/D3D/DXGI_Interface.h"
#include "Colorful/D3D/D3D11/D3D11Shader.h"
#include "Colorful/D3D/D3D11/D3D11TextureView.h"
#include "Colorful/D3D/D3D11/D3D11Pipeline.h"
#include "Colorful/D3D/D3D11/D3D11Buffer.h"

NAMESPACE_START(Gfx)

class D3D11Context1 : public D3DObject<ID3D11DeviceContext1> {};
class D3D11Context2 : public D3DObject<ID3D11DeviceContext2> {};
class D3D11Context3 : public D3DObject<ID3D11DeviceContext3> {};
class D3D11Context4 : public D3DObject<ID3D11DeviceContext4> {};

class D3D11Context : public D3DObject<ID3D11DeviceContext>
{
public:
	D3D11Context(ID3D11DeviceContext* handle);

	ID3D11DeviceContext1* get1()
	{
		assert(m_Context1.isValid());
		return m_Context1.get();
	}

	ID3D11DeviceContext2* get2()
	{
		assert(m_Context2.isValid());
		return m_Context2.get();
	}

	ID3D11DeviceContext3* get3()
	{
		assert(m_Context3.isValid());
		return m_Context3.get();
	}

	ID3D11DeviceContext4* get4()
	{
		assert(m_Context4.isValid());
		return m_Context4.get();
	}
protected:
private:
	D3D11Context1 m_Context1;
	D3D11Context2 m_Context2;
	D3D11Context3 m_Context3;
	D3D11Context4 m_Context4;
};

class D3D11Device1 : public D3DObject<ID3D11Device1> {};
class D3D11Device2 : public D3DObject<ID3D11Device2> {};
class D3D11Device3 : public D3DObject<ID3D11Device3> {};
class D3D11Device4 : public D3DObject<ID3D11Device4> {};
class D3D11Device5 : public D3DObject<ID3D11Device5> {};

class D3D11Device final : public D3DObject<ID3D11Device>, public IDevice
{
public:
	D3D11Device(IDXGIAdapter1* adapter1, IDXGIAdapter4* adapter4);

	D3D11Context* IMContext() const
	{
		return m_IMContext.get();
	}

	ID3D11Device1* get1()
	{
		assert(m_Device1.isValid());
		return m_Device1.get();
	}

	ID3D11Device2* get2()
	{
		assert(m_Device2.isValid());
		return m_Device2.get();
	}

	ID3D11Device3* get3()
	{
		assert(m_Device3.isValid());
		return m_Device3.get();
	}

	ID3D11Device4* get4()
	{
		assert(m_Device4.isValid());
		return m_Device4.get();
	}

	ID3D11Device5* get5()
	{
		assert(m_Device5.isValid());
		return m_Device5.get();
	}

	IShaderPtr createShader(ShaderDesc& desc) override final
	{
		return std::make_shared<D3D11Shader>(desc);
	}

	ITexturePtr createTexture(const TextureDesc& desc) override final
	{
	}

	ITexturePtr createRenderTarget(const TextureDesc& desc) override final
	{
		return std::make_shared<D3D11RenderTargetView>(get(), desc);
	}

	ITexturePtr createDepthStencil(const TextureDesc& desc) override final
	{
		return std::make_shared<D3D11DepthStencilView>(get(), desc);
	}

	IInputLayoutPtr createInputLayout(const InputLayoutDesc& desc, const ShaderDesc& binary) override final
	{
		return std::make_shared<D3D11InputLayout>(get(), desc, binary);
	}

	IBufferPtr createUniformBuffer(size_t size, const void* data) override final
	{
		return create_shared<D3D11Buffer>(get(), m_IMContext->get(), EBindFlags::UniformBuffer, EBufferUsage::Dynamic, size, data);
	}

	IBufferPtr createIndexBuffer(EBufferUsage usage, size_t size, const void* data) override final
	{
		return create_shared<D3D11Buffer>(get(), m_IMContext->get(), EBindFlags::IndexBuffer, usage, size, data);
	}

	IBufferPtr createVertexBuffer(EBufferUsage usage, size_t size, const void* data) override final
	{
		return create_shared<D3D11Buffer>(get(), m_IMContext->get(), EBindFlags::VertexBuffer, usage, size, data);
	}

	ICommandBufferPtr getActiveCommandBuffer() override final
	{
		return nullptr;
	}
protected:
	template<class FeatureData>
	inline FeatureData getFeatureData(D3D11_FEATURE feature)
	{
		FeatureData featureData;
		m_Handle->CheckFeatureSupport(feature, &featureData, sizeof(FeatureData));
		return featureData;
	}

	SamplerPtr createSampler_native(const SamplerDesc& desc) override final
	{
		return create_shared<D3D11Sampler>(get(), desc);
	}
private:
	std::unique_ptr<D3D11Context> m_IMContext = nullptr;

	D3D11Device1 m_Device1;
	D3D11Device2 m_Device2;
	D3D11Device3 m_Device3;
	D3D11Device4 m_Device4;
	D3D11Device5 m_Device5;
};

NAMESPACE_END(Gfx)
