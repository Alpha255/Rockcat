#include "RHI/D3D/D3D12/D3D12Texture.h"
#include "RHI/D3D/D3D12/D3D12Device.h"
#include "Core/Math/Color.h"

D3D12Texture::D3D12Texture(const D3D12Device& Device, const RHITextureCreateInfo& RHICreateInfo)
	: RHITexture(RHICreateInfo)
{
	DXGI_SAMPLE_DESC SampleDesc
	{
		.Count = static_cast<uint32_t>(RHICreateInfo.SampleCount),
		.Quality = 0u
	};

	D3D12_RESOURCE_DESC CreateDesc
	{
		.Dimension = ::GetDimension(RHICreateInfo.Dimension),
		.Alignment = 0u,
		.Width = RHICreateInfo.Width,
		.Height = RHICreateInfo.Height,
		.DepthOrArraySize = static_cast<uint16_t>(RHICreateInfo.Dimension == ERHITextureDimension::T_3D ? RHICreateInfo.Depth : RHICreateInfo.ArrayLayers),
		.MipLevels = static_cast<uint16_t>(RHICreateInfo.MipLevels),
		.Format = ::GetFormat(RHICreateInfo.Format),
		.SampleDesc = SampleDesc,
		.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		.Flags = D3D12_RESOURCE_FLAG_NONE
	};

	D3D12_CLEAR_VALUE ClearValue
	{
		.Format = CreateDesc.Format
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
		.Type = D3D12_HEAP_TYPE_DEFAULT,
		.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
		.CreationNodeMask = 0u,
		.VisibleNodeMask = 0u
	};

	assert(false); /// #TODO ResourceStates
	VERIFY_D3D(Device->CreateCommittedResource(
		&HeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&CreateDesc,
		D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON,
		&ClearValue,
		IID_PPV_ARGS(Reference())));

	D3DHwResource::SetDebugName(RHICreateInfo.Name.c_str());
}

D3D12Sampler::D3D12Sampler(const D3D12Device& Device, const RHISamplerCreateInfo& RHICreateInfo)
	: RHISampler(RHICreateInfo)
{
	assert(false);

	Math::Vector4 BorderColor = GetBorderColor(RHICreateInfo.BorderColor);

	D3D12_SAMPLER_DESC CreateDesc
	{
		.Filter = GetFilter(RHICreateInfo.MinMagFilter, RHICreateInfo.MipmapMode, static_cast<uint32_t>(RHICreateInfo.MaxAnisotropy)),
		.AddressU = GetSamplerAddressMode(RHICreateInfo.AddressModeU),
		.AddressV = GetSamplerAddressMode(RHICreateInfo.AddressModeV),
		.AddressW = GetSamplerAddressMode(RHICreateInfo.AddressModeW),
		.MipLODBias = RHICreateInfo.MipLODBias,
		.MaxAnisotropy = static_cast<uint32_t>(RHICreateInfo.MaxAnisotropy),
		.ComparisonFunc = GetCompareFunc(RHICreateInfo.CompareOp),
		.BorderColor = {BorderColor.x, BorderColor.y, BorderColor.z, BorderColor.w},
		.MinLOD = RHICreateInfo.MinLOD,
		.MaxLOD = RHICreateInfo.MaxLOD
	};

	Device->CreateSampler(&CreateDesc, m_Descriptor);
}
