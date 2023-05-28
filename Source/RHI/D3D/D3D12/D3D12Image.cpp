#include "Colorful/D3D/D3D12/D3D12Image.h"
#include "Colorful/D3D/D3D12/D3D12Device.h"

NAMESPACE_START(RHI)

D3D12Image::D3D12Image(D3D12Device* Device, const ImageDesc& Desc)
	: D3DHWObject(Desc)
{
	assert(Device);

	D3D12_RESOURCE_DESC CreateDesc
	{
		D3D12Type::ImageDimension(Desc.Type),
		0u,
		Desc.Width,
		Desc.Height,
		static_cast<uint16_t>(Desc.Type == EImageType::T_3D ? Desc.Depth : Desc.ArrayLayers),
		static_cast<uint16_t>(Desc.MipLevels),
		D3D12Type::Format(Desc.Format),
		DXGI_SAMPLE_DESC
		{
			static_cast<uint32_t>(Desc.SampleCount),
			0u
		},
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_NONE
	};

	D3D12_CLEAR_VALUE ClearValue
	{
		CreateDesc.Format
	};

	if (EnumHasAnyFlags(Desc.Usages, EBufferUsageFlags::RenderTarget))
	{
		CreateDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		ClearValue.Color[0] = Color::Black.x;
		ClearValue.Color[1] = Color::Black.y;
		ClearValue.Color[2] = Color::Black.z;
		ClearValue.Color[3] = Color::Black.w;
	}
	if (EnumHasAnyFlags(Desc.Usages, EBufferUsageFlags::DepthStencil))
	{
		CreateDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		ClearValue.DepthStencil.Depth = 1.0f;
		ClearValue.DepthStencil.Stencil = 0xF;
	}
	if (EnumHasAnyFlags(Desc.Usages, EBufferUsageFlags::UnorderedAccess))
	{
		CreateDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	}

	D3D12_HEAP_PROPERTIES HeapProperties
	{
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		0u,
		0u
	};

	assert(false); /// #TODO ResourceStates
	VERIFY_D3D(Device->Get()->CreateCommittedResource(
		&HeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&CreateDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON,
		&ClearValue,
		IID_PPV_ARGS(Reference())));

#if 1
	Get()->SetName(StringUtils::ToWide(Desc.Name).c_str());
#else
	SetDebugName(Desc.Name.c_str());
#endif
}

D3D12Sampler::D3D12Sampler(D3D12Device* Device, const SamplerDesc& Desc)
{
	assert(Device && false);

	Vector4 BorderColor = D3D12Type::BorderColor(Desc.BorderColor);

	D3D12_SAMPLER_DESC CreateDesc
	{
		D3D12Type::Filter(Desc.MinMagFilter, Desc.MipmapMode, Desc.MaxAnisotropy),
		D3D12Type::SamplerAddressMode(Desc.AddressModeU),
		D3D12Type::SamplerAddressMode(Desc.AddressModeV),
		D3D12Type::SamplerAddressMode(Desc.AddressModeW),
		Desc.MipLODBias,
		Desc.MaxAnisotropy,
		D3D12Type::CompareFunc(Desc.CompareOp),
		{BorderColor.x, BorderColor.y, BorderColor.z, BorderColor.w},
		Desc.MinLOD,
		Desc.MaxLOD
	};

	Device->Get()->CreateSampler(&CreateDesc, m_Descriptor);
}

D3D12Sampler::~D3D12Sampler()
{
}

NAMESPACE_END(RHI)
