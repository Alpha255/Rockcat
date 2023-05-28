#include "Colorful/D3D/D3D11/D3D11Texture.h"
#include "Colorful/D3D/D3D11/D3D11EnumTranslator.h"

NAMESPACE_START(Gfx)

D3D11Texture1D::D3D11Texture1D(ID3D11Device* device, const TextureDesc& desc)
{
	assert(device);
	assert(desc.Type == ETextureType::T_1D || desc.Type == ETextureType::T_1D_Array);

	std::vector<D3D11_SUBRESOURCE_DATA> subresources(desc.SubResources.size());
	for (int i = 0u; i < desc.SubResources.size(); ++i)
	{
		subresources[i] = D3D11_SUBRESOURCE_DATA
		{
			desc.Data + desc.SubResources[i].Offset,
			desc.SubResources[i].RowBytes,
			desc.SubResources[i].SliceBytes
		};
	}

	D3D11_TEXTURE1D_DESC createDesc
	{
		desc.Width,
		desc.MipLevels,
		desc.ArraySize,
		D3D11EnumTranslator::format(desc.Format),
		D3D11EnumTranslator::usage(desc.Usage),
		D3D11EnumTranslator::bindFlags(desc.BindFlags),
		0u,
		0u
	};
	VERIFY_D3D(device->CreateTexture1D(&createDesc, desc.Data ? subresources.data() : nullptr, reference()));
}

D3D11Texture2D::D3D11Texture2D(ID3D11Device* device, const TextureDesc& desc)
{
	assert(device);
	assert(desc.Type == ETextureType::T_2D || desc.Type == ETextureType::T_2D_Array);
	assert(desc.Type == ETextureType::T_Cube || desc.Type == ETextureType::T_Cube_Array);

	std::vector<D3D11_SUBRESOURCE_DATA> subresources(desc.SubResources.size());
	for (int i = 0u; i < desc.SubResources.size(); ++i)
	{
		subresources[i] = D3D11_SUBRESOURCE_DATA
		{
			desc.Data + desc.SubResources[i].Offset,
			desc.SubResources[i].RowBytes,
			desc.SubResources[i].SliceBytes
		};
	}

	D3D11_TEXTURE2D_DESC createDesc
	{
		desc.Width,
		desc.Height,
		desc.MipLevels,
		desc.ArraySize,
		D3D11EnumTranslator::format(desc.Format),
		{
			static_cast<uint32_t>(desc.SampleCount),
			0u
		},
		D3D11EnumTranslator::usage(desc.Usage),
		D3D11EnumTranslator::bindFlags(desc.BindFlags),
		0u,
		(desc.Type == ETextureType::T_Cube || desc.Type == ETextureType::T_Cube_Array) ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0u
	};
	VERIFY_D3D(device->CreateTexture2D(&createDesc, desc.Data ? subresources.data() : nullptr, reference()));
}

D3D11Texture3D::D3D11Texture3D(ID3D11Device* device, const TextureDesc& desc)
{
	assert(device);
	assert(desc.Type == ETextureType::T_3D);

	std::vector<D3D11_SUBRESOURCE_DATA> subresources(desc.SubResources.size());
	for (int i = 0u; i < desc.SubResources.size(); ++i)
	{
		subresources[i] = D3D11_SUBRESOURCE_DATA
		{
			desc.Data + desc.SubResources[i].Offset,
			desc.SubResources[i].RowBytes,
			desc.SubResources[i].SliceBytes
		};
	}

	D3D11_TEXTURE3D_DESC createDesc
	{
		desc.Width,
		desc.Height,
		desc.Depth,
		desc.MipLevels,
		D3D11EnumTranslator::format(desc.Format),
		D3D11EnumTranslator::usage(desc.Usage),
		D3D11EnumTranslator::bindFlags(desc.BindFlags),
		0u,
		0u
	};
	VERIFY_D3D(device->CreateTexture3D(&createDesc, desc.Data ? subresources.data() : nullptr, reference()));
}

D3D11Sampler::D3D11Sampler(ID3D11Device* device, const SamplerDesc& desc)
{
	assert(device);

	Vec4 borderColor;
	switch (desc.BorderColor)  /// ???? 
	{
	case EBorderColor::FloatTransparentBlack:
	case EBorderColor::IntTransparentBlack:
		break;
	case EBorderColor::FloatOpaqueBlack:
	case EBorderColor::IntOpaqueBlack:
		borderColor = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		break;
	case EBorderColor::FloatOpaqueWhite:
	case EBorderColor::IntOpaqueWhite:
		borderColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
		break;
	}

	D3D11_SAMPLER_DESC samplerDesc
	{
		D3D11EnumTranslator::filter(desc.MinMagFilter),
		D3D11EnumTranslator::samplerAddressMode(desc.AddressModeU),
		D3D11EnumTranslator::samplerAddressMode(desc.AddressModeV),
		D3D11EnumTranslator::samplerAddressMode(desc.AddressModeW),
		desc.MipLodBias,
		desc.MaxAnisotropy,
		D3D11EnumTranslator::compareFunc(desc.CompareOp),
		{
			borderColor.x,
			borderColor.y,
			borderColor.z,
			borderColor.w
		},
		desc.MinLod,
		desc.MaxLod
	};
	VERIFY_D3D(device->CreateSamplerState(&samplerDesc, reference()));
}

NAMESPACE_END(Gfx)
