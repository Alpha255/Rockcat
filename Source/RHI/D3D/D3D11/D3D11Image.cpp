#include "RHI/D3D/D3D11/D3D11Image.h"
#include "RHI/D3D/D3D11/D3D11Device.h"
#include "Runtime/Engine/Services/SpdLogService.h"

D3D11Image1D::D3D11Image1D(const D3D11Device& Device, const RHIImageCreateInfo& RHICreateInfo)
	: RHIImage(RHICreateInfo)
{
	assert(RHICreateInfo.ImageType == ERHIImageType::T_1D || RHICreateInfo.ImageType == ERHIImageType::T_1D_Array);

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

D3D11Image2D::D3D11Image2D(const D3D11Device& Device, const RHIImageCreateInfo& RHICreateInfo)
	: RHIImage(RHICreateInfo)
{
	assert(RHICreateInfo.ImageType == ERHIImageType::T_2D || 
		RHICreateInfo.ImageType == ERHIImageType::T_2D_Array ||
		RHICreateInfo.ImageType == ERHIImageType::T_Cube ||
		RHICreateInfo.ImageType == ERHIImageType::T_Cube_Array);

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
		(RHICreateInfo.ImageType == ERHIImageType::T_Cube || RHICreateInfo.ImageType == ERHIImageType::T_Cube_Array) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0u
	};
	VERIFY_D3D(Device->CreateTexture2D(&Desc, nullptr, Reference()));
}

D3D11Image3D::D3D11Image3D(const D3D11Device& Device, const RHIImageCreateInfo& RHICreateInfo)
	: RHIImage(RHICreateInfo)
{
	assert(RHICreateInfo.ImageType == ERHIImageType::T_3D);

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
{
	Math::Vector4 BorderColor = GetBorderColor(RHICreateInfo.BorderColor);

	D3D11_SAMPLER_DESC Desc
	{
		GetFilter(RHICreateInfo.MinMagFilter, RHICreateInfo.MipmapMode, RHICreateInfo.MaxAnisotropy),
		GetSamplerAddressMode(RHICreateInfo.AddressModeU),
		GetSamplerAddressMode(RHICreateInfo.AddressModeV),
		GetSamplerAddressMode(RHICreateInfo.AddressModeW),
		RHICreateInfo.MipLODBias,
		RHICreateInfo.MaxAnisotropy,
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
