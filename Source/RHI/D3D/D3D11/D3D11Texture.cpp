#include "RHI/D3D/D3D11/D3D11Texture.h"
#include "RHI/D3D/D3D11/D3D11Device.h"

D3D11Texture1D::D3D11Texture1D(const D3D11Device& Device, const RHITextureDesc& Desc)
	: RHITexture(Desc)
{
	assert(Desc.Dimension == ERHITextureDimension::T_1D || Desc.Dimension == ERHITextureDimension::T_1D_Array);

	std::vector<D3D11_SUBRESOURCE_DATA> SubresourceData;

	D3D11_TEXTURE1D_DESC Desc
	{
		Desc.Width,
		Desc.MipLevels,
		Desc.ArrayLayers,
		::GetFormat(Desc.Format),
		D3D11_USAGE_DEFAULT,
		0u,
		0u,
		0u
	};
	VERIFY_D3D(Device->CreateTexture1D(&Desc, nullptr, Reference()));
}

D3D11Texture2D::D3D11Texture2D(const D3D11Device& Device, const RHITextureDesc& Desc)
	: RHITexture(Desc)
{
	assert(Desc.Dimension == ERHITextureDimension::T_2D || 
		Desc.Dimension == ERHITextureDimension::T_2D_Array ||
		Desc.Dimension == ERHITextureDimension::T_Cube ||
		Desc.Dimension == ERHITextureDimension::T_Cube_Array);

	std::vector<D3D11_SUBRESOURCE_DATA> SubresourceData;

	D3D11_TEXTURE2D_DESC Desc
	{
		Desc.Width,
		Desc.Height,
		Desc.MipLevels,
		Desc.ArrayLayers,
		::GetFormat(Desc.Format),
		DXGI_SAMPLE_DESC
		{
			static_cast<uint32_t>(Desc.SampleCount),
			0u
		},
		D3D11_USAGE_DEFAULT,
		0u,
		0u,
		(Desc.Dimension == ERHITextureDimension::T_Cube || Desc.Dimension == ERHITextureDimension::T_Cube_Array) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0u
	};
	VERIFY_D3D(Device->CreateTexture2D(&Desc, nullptr, Reference()));
}

D3D11Texture3D::D3D11Texture3D(const D3D11Device& Device, const RHITextureDesc& Desc)
	: RHITexture(Desc)
{
	assert(Desc.Dimension == ERHITextureDimension::T_3D);

	std::vector<D3D11_SUBRESOURCE_DATA> SubresourceData;

	D3D11_TEXTURE3D_DESC Desc
	{
		Desc.Width,
		Desc.Height,
		Desc.Depth,
		Desc.ArrayLayers,
		::GetFormat(Desc.Format),
		D3D11_USAGE_DEFAULT,
		0u,
		0u,
		0u
	};
	VERIFY_D3D(Device->CreateTexture3D(&Desc, nullptr, Reference()));
}

D3D11Sampler::D3D11Sampler(const D3D11Device& Device, const RHISamplerDesc& Desc)
	: RHISampler(Desc)
{
	Math::Vector4 BorderColor = GetBorderColor(Desc.BorderColor);

	D3D11_SAMPLER_DESC Desc
	{
		GetFilter(Desc.MinMagFilter, Desc.MipmapMode, static_cast<uint32_t>(Desc.MaxAnisotropy)),
		GetSamplerAddressMode(Desc.AddressModeU),
		GetSamplerAddressMode(Desc.AddressModeV),
		GetSamplerAddressMode(Desc.AddressModeW),
		Desc.MipLODBias,
		static_cast<uint32_t>(Desc.MaxAnisotropy),
		GetCompareFunc(Desc.CompareOp),
		{
			BorderColor.x,
			BorderColor.y,
			BorderColor.z,
			BorderColor.w
		},
		Desc.MinLOD,
		Desc.MaxLOD
	};
	VERIFY_D3D(Device->CreateSamplerState(&Desc, Reference()));
}
