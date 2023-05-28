#include "Colorful/D3D/D3D11/D3D11TextureView.h"
#include "Colorful/D3D/D3D11/D3D11EnumTranslator.h"

NAMESPACE_START(Gfx)

ID3D11Resource* D3D11TextureView::createTexture(ID3D11Device* device, const TextureDesc& desc)
{
	assert(device);

	ID3D11Resource* resource = nullptr;
	if (desc.Type == ETextureType::Buffer)
	{
		assert(0);
	}
	else if (desc.Type == ETextureType::T_1D || desc.Type == ETextureType::T_1D_Array)
	{
		auto texture = std::make_shared<D3D11Texture1D>(device, desc);
		resource = texture->get();
		m_Texture = texture;
	}
	else if (desc.Type == ETextureType::T_2D || desc.Type == ETextureType::T_2D_Array ||
		desc.Type == ETextureType::T_Cube || desc.Type == ETextureType::T_Cube_Array)
	{
		auto texture = std::make_shared<D3D11Texture2D>(device, desc);
		resource = texture->get();
		m_Texture = texture;
	}
	else if (desc.Type == ETextureType::T_3D)
	{
		auto texture = std::make_shared<D3D11Texture3D>(device, desc);
		resource = texture->get();
		m_Texture = texture;
	}

	assert(resource);
	setDebugName(desc.Name.c_str());

	return resource;
}

D3D11ShaderResourceView::D3D11ShaderResourceView(ID3D11Device* device, const TextureDesc& desc)
	: D3D11TextureView(desc)
{
	assert(device && (desc.BindFlags & EBindFlags::ShaderResource));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc
	{
		D3D11EnumTranslator::format(desc.Format)
	};

	switch (desc.Type)
	{
	case ETextureType::Unknown:
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_UNKNOWN;
		break;
	case ETextureType::Buffer:
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER; /// D3D_SRV_DIMENSION_BUFFEREX Raw buffer???
		break;
	case ETextureType::T_1D:
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		shaderResourceViewDesc.Texture1D = D3D11_TEX1D_SRV
		{
			0u,
			desc.MipLevels
		};
		break;
	case ETextureType::T_1D_Array:
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
		shaderResourceViewDesc.Texture1DArray = D3D11_TEX1D_ARRAY_SRV
		{
			0u,
			desc.MipLevels,
			0u,
			desc.ArraySize
		};
		break;
	case ETextureType::T_2D:
		shaderResourceViewDesc.ViewDimension = desc.SampleCount > ESampleCount::Sample_1_Bit ? 
			D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		if (desc.SampleCount > ESampleCount::Sample_1_Bit)
		{
			shaderResourceViewDesc.Texture2DMS = D3D11_TEX2DMS_SRV
			{
				0u,
			};
		}
		else
		{
			shaderResourceViewDesc.Texture2D = D3D11_TEX2D_SRV
			{
				0u,
				desc.MipLevels
			};
		}
		break;
	case ETextureType::T_2D_Array:
		shaderResourceViewDesc.ViewDimension = desc.SampleCount > ESampleCount::Sample_1_Bit ? 
			D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		if (desc.SampleCount > ESampleCount::Sample_1_Bit)
		{
			shaderResourceViewDesc.Texture2DMSArray = D3D11_TEX2DMS_ARRAY_SRV
			{
				0u,
				desc.ArraySize
			};
		}
		else
		{
			shaderResourceViewDesc.Texture2DArray = D3D11_TEX2D_ARRAY_SRV
			{
				0u,
				desc.MipLevels,
				0u,
				desc.ArraySize
			};
		}
		break;
	case ETextureType::T_3D:
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		shaderResourceViewDesc.Texture3D = D3D11_TEX3D_SRV
		{
			0u,
			desc.MipLevels
		};
		break;
	case ETextureType::T_Cube:
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		shaderResourceViewDesc.TextureCube = D3D11_TEXCUBE_SRV
		{
			0u,
			desc.MipLevels
		};
		break;
	case ETextureType::T_Cube_Array:
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		shaderResourceViewDesc.TextureCubeArray = D3D11_TEXCUBE_ARRAY_SRV
		{
			0u,
			desc.MipLevels,
			0u,
			desc.ArraySize
		};
		break;
	}

	VERIFY_D3D(device->CreateShaderResourceView(createTexture(device, desc), &shaderResourceViewDesc, reference()));
}

D3D11RenderTargetView::D3D11RenderTargetView(ID3D11Device* device, const TextureDesc& desc)
	: D3D11TextureView(desc, EDescriptorType::InputAttachment)
{
	assert(device && (desc.BindFlags & EBindFlags::RenderTarget));

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc
	{
		D3D11EnumTranslator::format(desc.Format)
	};

	switch (desc.Type)
	{
	case ETextureType::Buffer:
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_BUFFER; /// D3D_SRV_DIMENSION_BUFFEREX Raw buffer???
		break;
	case ETextureType::T_1D:
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
		renderTargetViewDesc.Texture1D = D3D11_TEX1D_RTV
		{
			0u
		};
		break;
	case ETextureType::T_1D_Array:
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
		renderTargetViewDesc.Texture1DArray = D3D11_TEX1D_ARRAY_RTV
		{
			0u,
			0u,
			desc.ArraySize
		};
		break;
	case ETextureType::T_2D:
		renderTargetViewDesc.ViewDimension = desc.SampleCount > ESampleCount::Sample_1_Bit ? 
			D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		if (desc.SampleCount > ESampleCount::Sample_1_Bit)
		{
			renderTargetViewDesc.Texture2DMS = D3D11_TEX2DMS_RTV
			{
				0u,
			};
		}
		else
		{
			renderTargetViewDesc.Texture2D = D3D11_TEX2D_RTV
			{
				0u,
			};
		}
		break;
	case ETextureType::T_2D_Array:
		renderTargetViewDesc.ViewDimension = desc.SampleCount > ESampleCount::Sample_1_Bit ? 
			D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		if (desc.SampleCount > ESampleCount::Sample_1_Bit)
		{
			renderTargetViewDesc.Texture2DMSArray = D3D11_TEX2DMS_ARRAY_RTV
			{
				0u,
				desc.ArraySize
			};
		}
		else
		{
			renderTargetViewDesc.Texture2DArray = D3D11_TEX2D_ARRAY_RTV
			{
				0u,
				0u,
				desc.ArraySize
			};
		}
		break;
	case ETextureType::T_3D:
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
		renderTargetViewDesc.Texture3D = D3D11_TEX3D_RTV
		{
			0u,
			0u,
			desc.Depth
		};
		break;
	default:
		assert(0);
		break;
	}

	VERIFY_D3D(device->CreateRenderTargetView(createTexture(device, desc), &renderTargetViewDesc, reference()));
}

D3D11DepthStencilView::D3D11DepthStencilView(ID3D11Device* device, const TextureDesc& desc)
	: D3D11TextureView(desc, EDescriptorType::InputAttachment)
{
	assert(device && (desc.BindFlags & EBindFlags::DepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc
	{
		D3D11EnumTranslator::format(desc.Format),
		D3D11_DSV_DIMENSION_UNKNOWN,
		0u /// A value that describes whether the texture is read only. Pass 0 to specify that it is not read only; otherwise, pass one of the members of the D3D11_DSV_READ_ONLY_DEPTH/D3D11_DSV_READ_ONLY_STENCIL enumerated type.
	};

	switch (desc.Type)
	{
	case ETextureType::T_1D:
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
		depthStencilViewDesc.Texture1D = D3D11_TEX1D_DSV
		{
			0u
		};
		break;
	case ETextureType::T_1D_Array:
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
		depthStencilViewDesc.Texture1DArray = D3D11_TEX1D_ARRAY_DSV
		{
			0u,
			0u,
			desc.ArraySize
		};
		break;
	case ETextureType::T_2D:
		depthStencilViewDesc.ViewDimension = desc.SampleCount > ESampleCount::Sample_1_Bit ? 
			D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		if (desc.SampleCount > ESampleCount::Sample_1_Bit)
		{
			depthStencilViewDesc.Texture2DMS = D3D11_TEX2DMS_DSV
			{
				0u,
			};
		}
		else
		{
			depthStencilViewDesc.Texture2D = D3D11_TEX2D_DSV
			{
				0u,
			};
		}
		break;
	case ETextureType::T_2D_Array:
		depthStencilViewDesc.ViewDimension = desc.SampleCount > ESampleCount::Sample_1_Bit ? 
			D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		if (desc.SampleCount > ESampleCount::Sample_1_Bit)
		{
			depthStencilViewDesc.Texture2DMSArray = D3D11_TEX2DMS_ARRAY_DSV
			{
				0u,
				desc.ArraySize
			};
		}
		else
		{
			depthStencilViewDesc.Texture2DArray = D3D11_TEX2D_ARRAY_DSV
			{
				0u,
				0u,
				desc.ArraySize
			};
		}
		break;
	default:
		assert(0);
		break;
	}

	VERIFY_D3D(device->CreateDepthStencilView(createTexture(device, desc), &depthStencilViewDesc, reference()));
}

D3D11UnorderedAccessView::D3D11UnorderedAccessView(ID3D11Device* device)
{
	assert(0);
	assert(device);

	D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc
	{
	};

	VERIFY_D3D(device->CreateUnorderedAccessView(nullptr, &unorderedAccessViewDesc, reference()));
}

NAMESPACE_END(Gfx)
