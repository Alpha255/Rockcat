#pragma once

#include "Colorful/D3D/D3D11/D3D11EnumTranslator.h"

NAMESPACE_START(Gfx)

class D3D11VertexShader final : public D3DObject<ID3D11VertexShader>, public IShader
{
public:
	D3D11VertexShader(ID3D11Device* device, ShaderDesc& desc)
		: IShader(EShaderStage::Vertex, desc.ResourceList)
	{
		assert(device && desc.BinarySize);
		VERIFY_D3D(device->CreateVertexShader(desc.Binary.get(), desc.BinarySize, nullptr, reference()));
	}
};

class D3D11HullShader final : public D3DObject<ID3D11HullShader>, public IShader
{
public:
	D3D11HullShader(ID3D11Device* device, ShaderDesc& desc)
		: IShader(EShaderStage::Hull, desc.ResourceList)
	{
		assert(device && desc.BinarySize);
		VERIFY_D3D(device->CreateHullShader(desc.Binary.get(), desc.BinarySize, nullptr, reference()));
	}
};

class D3D11DomainShader final : public D3DObject<ID3D11DomainShader>, public IShader
{
public:
	D3D11DomainShader(ID3D11Device* device, ShaderDesc& desc)
		: IShader(EShaderStage::Domain, desc.ResourceList)
	{
		assert(device && desc.BinarySize);
		VERIFY_D3D(device->CreateDomainShader(desc.Binary.get(), desc.BinarySize, nullptr, reference()));
	}
};

class D3D11GeometryShader final : public D3DObject<ID3D11GeometryShader>, public IShader
{
public:
	D3D11GeometryShader(ID3D11Device* device, ShaderDesc& desc)
		: IShader(EShaderStage::Geometry, desc.ResourceList)
	{
		assert(device && desc.BinarySize);
		VERIFY_D3D(device->CreateGeometryShader(desc.Binary.get(), desc.BinarySize, nullptr, reference()));
	}
};

class D3D11FragmentShader final : public D3DObject<ID3D11PixelShader>, public IShader
{
public:
	D3D11FragmentShader(ID3D11Device* device, ShaderDesc& desc)
		: IShader(EShaderStage::Fragment, desc.ResourceList)
	{
		assert(device && desc.BinarySize);
		VERIFY_D3D(device->CreatePixelShader(desc.Binary.get(), desc.BinarySize, nullptr, reference()));
	}
};

class D3D11ComputeShader final : public D3DObject<ID3D11ComputeShader>, public IShader
{
public:
	D3D11ComputeShader(ID3D11Device* device, ShaderDesc& desc)
		: IShader(EShaderStage::Fragment, desc.ResourceList)
	{
		assert(device && desc.BinarySize);
		VERIFY_D3D(device->CreateComputeShader(desc.Binary.get(), desc.BinarySize, nullptr, reference()));
	}
};

class D3D11Shader final : public D3DObject<ID3D11DeviceChild>, public IShader
{
public:
	D3D11Shader(ID3D11Device* device, ShaderDesc& desc)
		: IShader(desc.Stage, desc.ResourceList)
	{
		assert(device && desc.BinarySize);

		switch (desc.Stage)
		{
		case EShaderStage::Vertex:
			VERIFY_D3D(device->CreateVertexShader(desc.Binary.get(), desc.BinarySize, nullptr, reinterpret_cast<ID3D11VertexShader**>(reference())));
			m_Shader.VertexShader = reinterpret_cast<ID3D11VertexShader*>(get());
			break;
		case EShaderStage::Hull:
			VERIFY_D3D(device->CreateHullShader(desc.Binary.get(), desc.BinarySize, nullptr, reinterpret_cast<ID3D11HullShader**>(reference())));
			m_Shader.HullShader = reinterpret_cast<ID3D11HullShader*>(get());
			break;
		case EShaderStage::Domain:
			VERIFY_D3D(device->CreateDomainShader(desc.Binary.get(), desc.BinarySize, nullptr, reinterpret_cast<ID3D11DomainShader**>(reference())));
			m_Shader.DomainShader = reinterpret_cast<ID3D11DomainShader*>(get());
			break;
		case EShaderStage::Geometry:
			VERIFY_D3D(device->CreateGeometryShader(desc.Binary.get(), desc.BinarySize, nullptr, reinterpret_cast<ID3D11GeometryShader**>(reference())));
			m_Shader.GeometryShader = reinterpret_cast<ID3D11GeometryShader*>(get());
			break;
		case EShaderStage::Fragment:
			VERIFY_D3D(device->CreatePixelShader(desc.Binary.get(), desc.BinarySize, nullptr, reinterpret_cast<ID3D11PixelShader**>(reference())));
			m_Shader.FragmentShader = reinterpret_cast<ID3D11PixelShader*>(get());
			break;
		case EShaderStage::Compute:
			VERIFY_D3D(device->CreateComputeShader(desc.Binary.get(), desc.BinarySize, nullptr, reinterpret_cast<ID3D11ComputeShader**>(reference())));
			m_Shader.ComputeShader = reinterpret_cast<ID3D11ComputeShader*>(get());
			break;
		default:
			assert(0);
			break;
		}
	}

	ID3D11VertexShader* vs() const
	{
		assert(m_Stage == EShaderStage::Vertex);
		return m_Shader.VertexShader;
	}

	ID3D11HullShader* hs() const
	{
		assert(m_Stage == EShaderStage::Hull);
		return m_Shader.HullShader;
	}

	ID3D11DomainShader* ds() const
	{
		assert(m_Stage == EShaderStage::Domain);
		return m_Shader.DomainShader;
	}

	ID3D11GeometryShader* gs() const
	{
		assert(m_Stage == EShaderStage::Geometry);
		return m_Shader.GeometryShader;
	}

	ID3D11PixelShader* fs() const
	{
		assert(m_Stage == EShaderStage::Fragment);
		return m_Shader.FragmentShader;
	}

	ID3D11ComputeShader* cs() const
	{
		assert(m_Stage == EShaderStage::Compute);
		return m_Shader.ComputeShader;
	}
private:
	union 
	{
		ID3D11VertexShader* VertexShader;
		ID3D11HullShader* HullShader;
		ID3D11DomainShader* DomainShader;
		ID3D11GeometryShader* GeometryShader;
		ID3D11PixelShader* FragmentShader;
		ID3D11ComputeShader* ComputeShader;
	} m_Shader{};
};

class D3D11InputLayout final : public D3DObject<ID3D11InputLayout>, public IInputLayout
{
public:
	D3D11InputLayout(ID3D11Device* device, const InputLayoutDesc& desc, const ShaderDesc& shaderDesc)
	{
		assert(device && shaderDesc.BinarySize);

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
		for (uint32_t i = 0u; i < desc.Bindings.size(); ++i)
		{
			for (uint32_t j = 0u; j < desc.Bindings[i].Attributes.size(); ++j)
			{
				assert(desc.Bindings[i].Binding < ELimitations::MaxVertexStreams);

				auto& layout = desc.Bindings[i].Attributes[j];
				D3D11_INPUT_ELEMENT_DESC inputElement
				{
					shaderDesc.Language == EShaderLanguage::HLSL ? layout.Usage.c_str() : "TEXCOORD",
					shaderDesc.Language == EShaderLanguage::HLSL ? 0u : j,
					D3D11EnumTranslator::format(layout.Format),
					desc.Bindings[i].Binding,
					D3D11_APPEND_ALIGNED_ELEMENT,  /// Define the current element directly after the previous one, including any packing if necessary
					desc.Bindings[i].InputRate == EVertexInputRate::Instance ?
						D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA,
					desc.Bindings[i].InputRate == EVertexInputRate::Instance ? 1u : 0u  /// ??? 
				};
				inputElements.emplace_back(std::move(inputElement));
			}
		}

		VERIFY_D3D(device->CreateInputLayout(
			inputElements.data(),
			static_cast<uint32_t>(inputElements.size()),
			shaderDesc.Binary.get(),
			shaderDesc.BinarySize,
			reference()));
	}
};

NAMESPACE_END(Gfx)