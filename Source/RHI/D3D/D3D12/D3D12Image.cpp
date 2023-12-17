#include "RHI/D3D/D3D12/D3D12Image.h"
#include "RHI/D3D/D3D12/D3D12Device.h"
#include "Runtime/Engine/Services/SpdLogService.h"
#include "Runtime/Core/Math/Color.h"

D3D12Image::D3D12Image(const D3D12Device& Device, const RHIImageCreateInfo& RHICreateInfo)
	: RHIImage(RHICreateInfo)
{
	D3D12_RESOURCE_DESC CreateDesc
	{
		GetImageDimension(RHICreateInfo.ImageType),
		0u,
		RHICreateInfo.Width,
		RHICreateInfo.Height,
		static_cast<uint16_t>(RHICreateInfo.ImageType == ERHIImageType::T_3D ? RHICreateInfo.Depth : RHICreateInfo.ArrayLayers),
		static_cast<uint16_t>(RHICreateInfo.MipLevels),
		::GetFormat(RHICreateInfo.Format),
		DXGI_SAMPLE_DESC
		{
			static_cast<uint32_t>(RHICreateInfo.SampleCount),
			0u
		},
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_NONE
	};

	D3D12_CLEAR_VALUE ClearValue
	{
		CreateDesc.Format
	};

	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::RenderTarget))
	{
		CreateDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		ClearValue.Color[0] = Math::Color::Black.x;
		ClearValue.Color[1] = Math::Color::Black.y;
		ClearValue.Color[2] = Math::Color::Black.z;
		ClearValue.Color[3] = Math::Color::Black.w;
	}
	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::DepthStencil))
	{
		CreateDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		ClearValue.DepthStencil.Depth = 1.0f;
		ClearValue.DepthStencil.Stencil = 0xF;
	}
	if (EnumHasAnyFlags(RHICreateInfo.BufferUsageFlags, ERHIBufferUsageFlags::UnorderedAccess))
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
	VERIFY_D3D(Device->CreateCommittedResource(
		&HeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&CreateDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON,
		&ClearValue,
		IID_PPV_ARGS(Reference())));

	SetDebugName(RHICreateInfo.Name.c_str());
}

D3D12Sampler::D3D12Sampler(const D3D12Device& Device, const RHISamplerCreateInfo& RHICreateInfo)
{
	assert(false);

	Math::Vector4 BorderColor = GetBorderColor(RHICreateInfo.BorderColor);

	D3D12_SAMPLER_DESC CreateDesc
	{
		GetFilter(RHICreateInfo.MinMagFilter, RHICreateInfo.MipmapMode, RHICreateInfo.MaxAnisotropy),
		GetSamplerAddressMode(RHICreateInfo.AddressModeU),
		GetSamplerAddressMode(RHICreateInfo.AddressModeV),
		GetSamplerAddressMode(RHICreateInfo.AddressModeW),
		RHICreateInfo.MipLODBias,
		RHICreateInfo.MaxAnisotropy,
		GetCompareFunc(RHICreateInfo.CompareOp),
		{BorderColor.x, BorderColor.y, BorderColor.z, BorderColor.w},
		RHICreateInfo.MinLOD,
		RHICreateInfo.MaxLOD
	};

	Device->CreateSampler(&CreateDesc, m_Descriptor);
}
