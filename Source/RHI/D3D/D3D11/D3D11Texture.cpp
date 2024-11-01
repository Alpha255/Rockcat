#include "RHI/D3D/D3D11/D3D11Texture.h"
#include "RHI/D3D/D3D11/D3D11Device.h"
#include "Engine/Services/SpdLogService.h"

D3D11Texture1D::D3D11Texture1D(const D3D11Device& Device, const RHITextureCreateInfo& RHICreateInfo)
	: RHITexture(RHICreateInfo)
{
	assert(RHICreateInfo.Dimension == ERHITextureDimension::T_1D || RHICreateInfo.Dimension == ERHITextureDimension::T_1D_Array);

	std::vector<D3D11_SUBRESOURCE_DATA> SubresourceData;

	D3D11_TEXTURE1D_DESC Desc
	{
		RHICreateInfo.Width,
		RHICreateInfo.MipLevels,
		RHICreateInfo.ArrayLayers,
		::GetFormat(RHICreateInfo.Format),
		D3D11_USAGE_DEFAULT,
		0u,
		0u,
		0u
	};
	VERIFY_D3D(Device->CreateTexture1D(&Desc, nullptr, Reference()));
}

D3D11Texture2D::D3D11Texture2D(const D3D11Device& Device, const RHITextureCreateInfo& RHICreateInfo)
	: RHITexture(RHICreateInfo)
{
	assert(RHICreateInfo.Dimension == ERHITextureDimension::T_2D || 
		RHICreateInfo.Dimension == ERHITextureDimension::T_2D_Array ||
		RHICreateInfo.Dimension == ERHITextureDimension::T_Cube ||
		RHICreateInfo.Dimension == ERHITextureDimension::T_Cube_Array);

	std::vector<D3D11_SUBRESOURCE_DATA> SubresourceData;

	D3D11_TEXTURE2D_DESC Desc
	{
		RHICreateInfo.Width,
		RHICreateInfo.Height,
		RHICreateInfo.MipLevels,
		RHICreateInfo.ArrayLayers,
		::GetFormat(RHICreateInfo.Format),
		DXGI_SAMPLE_DESC
		{
			static_cast<uint32_t>(RHICreateInfo.SampleCount),
			0u
		},
		D3D11_USAGE_DEFAULT,
		0u,
		0u,
		(RHICreateInfo.Dimension == ERHITextureDimension::T_Cube || RHICreateInfo.Dimension == ERHITextureDimension::T_Cube_Array) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0u
	};
	VERIFY_D3D(Device->CreateTexture2D(&Desc, nullptr, Reference()));
}

D3D11Texture3D::D3D11Texture3D(const D3D11Device& Device, const RHITextureCreateInfo& RHICreateInfo)
	: RHITexture(RHICreateInfo)
{
	assert(RHICreateInfo.Dimension == ERHITextureDimension::T_3D);

	std::vector<D3D11_SUBRESOURCE_DATA> SubresourceData;

	D3D11_TEXTURE3D_DESC Desc
	{
		RHICreateInfo.Width,
		RHICreateInfo.Height,
		RHICreateInfo.Depth,
		RHICreateInfo.ArrayLayers,
		::GetFormat(RHICreateInfo.Format),
		D3D11_USAGE_DEFAULT,
		0u,
		0u,
		0u
	};
	VERIFY_D3D(Device->CreateTexture3D(&Desc, nullptr, Reference()));
}

D3D11Sampler::D3D11Sampler(const D3D11Device& Device, const RHISamplerCreateInfo& RHICreateInfo)
	: RHISampler(RHICreateInfo)
{
	Math::Vector4 BorderColor = GetBorderColor(RHICreateInfo.BorderColor);

	D3D11_SAMPLER_DESC Desc
	{
		GetFilter(RHICreateInfo.MinMagFilter, RHICreateInfo.MipmapMode, static_cast<uint32_t>(RHICreateInfo.MaxAnisotropy)),
		GetSamplerAddressMode(RHICreateInfo.AddressModeU),
		GetSamplerAddressMode(RHICreateInfo.AddressModeV),
		GetSamplerAddressMode(RHICreateInfo.AddressModeW),
		RHICreateInfo.MipLODBias,
		static_cast<uint32_t>(RHICreateInfo.MaxAnisotropy),
		GetCompareFunc(RHICreateInfo.CompareOp),
		{
			BorderColor.x,
			BorderColor.y,
			BorderColor.z,
			BorderColor.w
		},
		RHICreateInfo.MinLOD,
		RHICreateInfo.MaxLOD
	};
	VERIFY_D3D(Device->CreateSamplerState(&Desc, Reference()));
}
