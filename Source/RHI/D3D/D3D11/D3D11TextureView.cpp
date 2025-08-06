#include "RHI/D3D/D3D11/D3D11TextureView.h"
#include "RHI/D3D/D3D11/D3D11Device.h"

D3D11ShaderResourceView::D3D11ShaderResourceView(const D3D11Device& Device, const RHITextureDesc& Desc)
{
	assert(EnumHasAnyFlags(Desc.BufferUsageFlags, ERHIBufferUsageFlags::ShaderResource));

	D3D11_SHADER_RESOURCE_VIEW_DESC Desc
	{
		::GetFormat(Desc.Format)
	};

	switch (Desc.Dimension)
	{
	case ERHITextureDimension::Unknown:
		Desc.ViewDimension = D3D11_SRV_DIMENSION_UNKNOWN;
		break;
	case ERHITextureDimension::Buffer:
		Desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER; /// D3D_SRV_DIMENSION_BUFFEREX Raw buffer???
		break;
	case ERHITextureDimension::T_1D:
		Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
		Desc.Texture1D = D3D11_TEX1D_SRV
		{
			0u,
			Desc.MipLevels
		};
		break;
	case ERHITextureDimension::T_1D_Array:
		Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
		Desc.Texture1DArray = D3D11_TEX1D_ARRAY_SRV
		{
			0u,
			Desc.MipLevels,
			0u,
			Desc.ArrayLayers
		};
		break;
	case ERHITextureDimension::T_2D:
		Desc.ViewDimension = Desc.SampleCount > ERHISampleCount::Sample_1_Bit ?
			D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
		if (Desc.SampleCount > ERHISampleCount::Sample_1_Bit)
		{
			Desc.Texture2DMS = D3D11_TEX2DMS_SRV
			{
				0u,
			};
		}
		else
		{
			Desc.Texture2D = D3D11_TEX2D_SRV
			{
				0u,
				Desc.MipLevels
			};
		}
		break;
	case ERHITextureDimension::T_2D_Array:
		Desc.ViewDimension = Desc.SampleCount > ERHISampleCount::Sample_1_Bit ?
			D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY : D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		if (Desc.SampleCount > ERHISampleCount::Sample_1_Bit)
		{
			Desc.Texture2DMSArray = D3D11_TEX2DMS_ARRAY_SRV
			{
				0u,
				Desc.ArrayLayers
			};
		}
		else
		{
			Desc.Texture2DArray = D3D11_TEX2D_ARRAY_SRV
			{
				0u,
				Desc.MipLevels,
				0u,
				Desc.ArrayLayers
			};
		}
		break;
	case ERHITextureDimension::T_3D:
		Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
		Desc.Texture3D = D3D11_TEX3D_SRV
		{
			0u,
			Desc.MipLevels
		};
		break;
	case ERHITextureDimension::T_Cube:
		Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		Desc.TextureCube = D3D11_TEXCUBE_SRV
		{
			0u,
			Desc.MipLevels
		};
		break;
	case ERHITextureDimension::T_Cube_Array:
		Desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		Desc.TextureCubeArray = D3D11_TEXCUBE_ARRAY_SRV
		{
			0u,
			Desc.MipLevels,
			0u,
			Desc.ArrayLayers
		};
		break;
	}

	VERIFY_D3D(Device->CreateShaderResourceView(nullptr, &Desc, Reference()));
}

D3D11RenderTargetView::D3D11RenderTargetView(const D3D11Device& Device, const RHITextureDesc& Desc)
{
	assert(EnumHasAnyFlags(Desc.BufferUsageFlags, ERHIBufferUsageFlags::RenderTarget));

	D3D11_RENDER_TARGET_VIEW_DESC Desc
	{
		::GetFormat(Desc.Format)
	};

	switch (Desc.Dimension)
	{
	case ERHITextureDimension::Buffer:
		Desc.ViewDimension = D3D11_RTV_DIMENSION_BUFFER; /// D3D_SRV_DIMENSION_BUFFEREX Raw buffer???
		break;
	case ERHITextureDimension::T_1D:
		Desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
		Desc.Texture1D = D3D11_TEX1D_RTV
		{
			0u
		};
		break;
	case ERHITextureDimension::T_1D_Array:
		Desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
		Desc.Texture1DArray = D3D11_TEX1D_ARRAY_RTV
		{
			0u,
			0u,
			Desc.ArrayLayers
		};
		break;
	case ERHITextureDimension::T_2D:
		Desc.ViewDimension = Desc.SampleCount > ERHISampleCount::Sample_1_Bit ?
			D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		if (Desc.SampleCount > ERHISampleCount::Sample_1_Bit)
		{
			Desc.Texture2DMS = D3D11_TEX2DMS_RTV
			{
				0u,
			};
		}
		else
		{
			Desc.Texture2D = D3D11_TEX2D_RTV
			{
				0u,
			};
		}
		break;
	case ERHITextureDimension::T_2D_Array:
		Desc.ViewDimension = Desc.SampleCount > ERHISampleCount::Sample_1_Bit ?
			D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY : D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		if (Desc.SampleCount > ERHISampleCount::Sample_1_Bit)
		{
			Desc.Texture2DMSArray = D3D11_TEX2DMS_ARRAY_RTV
			{
				0u,
				Desc.ArrayLayers
			};
		}
		else
		{
			Desc.Texture2DArray = D3D11_TEX2D_ARRAY_RTV
			{
				0u,
				0u,
				Desc.ArrayLayers
			};
		}
		break;
	case ERHITextureDimension::T_3D:
		Desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
		Desc.Texture3D = D3D11_TEX3D_RTV
		{
			0u,
			0u,
			Desc.Depth
		};
		break;
	default:
		assert(0);
		break;
	}

	VERIFY_D3D(Device->CreateRenderTargetView(nullptr, &Desc, Reference()));
}

D3D11DepthStencilView::D3D11DepthStencilView(const D3D11Device& Device, const RHITextureDesc& Desc)
{
	assert(EnumHasAnyFlags(Desc.BufferUsageFlags, ERHIBufferUsageFlags::DepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC Desc
	{
		::GetFormat(Desc.Format),
		D3D11_DSV_DIMENSION_UNKNOWN,
		0u /// A value that describes whether the texture is read only. Pass 0 to specify that it is not read only; otherwise, pass one of the members of the D3D11_DSV_READ_ONLY_DEPTH/D3D11_DSV_READ_ONLY_STENCIL enumerated type.
	};

	switch (Desc.Dimension)
	{
	case ERHITextureDimension::T_1D:
		Desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
		Desc.Texture1D = D3D11_TEX1D_DSV
		{
			0u
		};
		break;
	case ERHITextureDimension::T_1D_Array:
		Desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
		Desc.Texture1DArray = D3D11_TEX1D_ARRAY_DSV
		{
			0u,
			0u,
			Desc.ArrayLayers
		};
		break;
	case ERHITextureDimension::T_2D:
		Desc.ViewDimension = Desc.SampleCount > ERHISampleCount::Sample_1_Bit ?
			D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		if (Desc.SampleCount > ERHISampleCount::Sample_1_Bit)
		{
			Desc.Texture2DMS = D3D11_TEX2DMS_DSV
			{
				0u,
			};
		}
		else
		{
			Desc.Texture2D = D3D11_TEX2D_DSV
			{
				0u,
			};
		}
		break;
	case ERHITextureDimension::T_2D_Array:
		Desc.ViewDimension = Desc.SampleCount > ERHISampleCount::Sample_1_Bit ?
			D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY : D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		if (Desc.SampleCount > ERHISampleCount::Sample_1_Bit)
		{
			Desc.Texture2DMSArray = D3D11_TEX2DMS_ARRAY_DSV
			{
				0u,
				Desc.ArrayLayers
			};
		}
		else
		{
			Desc.Texture2DArray = D3D11_TEX2D_ARRAY_DSV
			{
				0u,
				0u,
				Desc.ArrayLayers
			};
		}
		break;
	default:
		assert(0);
		break;
	}

	VERIFY_D3D(Device->CreateDepthStencilView(nullptr, &Desc, Reference()));
}

D3D11UnorderedAccessView::D3D11UnorderedAccessView(const D3D11Device& Device)
{
	assert(0);

	D3D11_UNORDERED_ACCESS_VIEW_DESC Desc
	{
	};

	VERIFY_D3D(Device->CreateUnorderedAccessView(nullptr, &Desc, Reference()));
}
