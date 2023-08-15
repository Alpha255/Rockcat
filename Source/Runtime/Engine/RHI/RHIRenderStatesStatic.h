#pragma once

#include "Runtime/Engine/RHI/RHIRenderStates.h"
#include "Runtime/Engine/RHI/RHIImage.h"

template<class RHIState>
class RHIStaticState : public RHIResource
{
protected:
	static std::shared_ptr<RHIState> s_RHIState;
};

template<class RHIState>
std::shared_ptr<RHIState> RHIStaticState<RHIState>::s_RHIState;

template<
	ERHIFilter MinMagFilter = ERHIFilter::Nearest,
	ERHIFilter MipmapMode = ERHIFilter::Nearest,
	ESamplerReduction Reduction = ESamplerReduction::Standard,
	ERHISamplerAddressMode AddressModeU = ERHISamplerAddressMode::Repeat,
	ERHISamplerAddressMode AddressModeV = ERHISamplerAddressMode::Repeat,
	ERHISamplerAddressMode AddressModeW = ERHISamplerAddressMode::Repeat,
	ERHICompareFunc CompareOp = ERHICompareFunc::Less,
	ERHIBorderColor BorderColor = ERHIBorderColor::FloatTransparentBlack,
	uint32_t MaxAnisotropy = 0u>
class RHISamplerStatic : public RHIStaticState<RHISampler>
{
public:
	static RHISamplerPtr Get(RHIDevice* Device)
	{
		if (!s_RHIState)
		{
			RHISamplerCreateInfo CreateInfo;
			CreateInfo.SetMinMagFilter(MinMagFilter)
				.SetMipmapMode(MipmapMode)
				.SetSamplerReduction(Reduction)
				.SetAddressModeU(AddressModeU)
				.SetAddressModeV(AddressModeV)
				.SetAddressModeW(AddressModeW)
				.SetCompareOp(CompareOp)
				.SetBorderColor(BorderColor)
				.SetMaxAnisotropy(static_cast<float32_t>(MaxAnisotropy))
				.SetMipLODBias(0.0f)
				.SetMinLOD(0.0f)
				.SetMaxLOD(0.0f);
			s_RHIState = Device->CreateSampler(CreateInfo);
		}
	}
};

class RHIRasterizationState
{
};

class RHIDepthStencilState
{
};

class RHIBlendState
{
};

template<
	ERHIPolygonMode PolygonMode = ERHIPolygonMode::Solid,
	ERHICullMode CullMode = ERHICullMode::BackFace,
	ERHIFrontFace FrontFace = ERHIFrontFace::Clockwise,
	bool8_t EnableDepthClamp = false>
class RHIRasterizationStateStatic : public RHIStaticState<RHIRasterizationState>
{
public:
	static std::shared_ptr<RHIRasterizationState> Get(RHIDevice* Device)
	{
		RHIRasterizationStateCreateInfo CreateInfo;
		CreateInfo.SetPolygonMode(PolygonMode)
			.SetCullMode(CullMode)
			.SetFrontFace(FrontFace)
			.SetEnableDepthClamp(EnableDepthClamp);
	}
};

template<
	bool8_t EnableDepth = true,
	bool8_t EnableDepthWrite = false,
	bool8_t EnableDepthBoundsTest = false,
	bool8_t EnableStencil = false,
	ERHICompareFunc DepthCompareFunc = ERHICompareFunc::Less,
	uint8_t StencilReadMask = 0xFF,
	uint8_t StencilWriteMask = 0xFF,
	ERHIStencilOp FailOpFrontFace = ERHIStencilOp::Keep,
	ERHIStencilOp PassOpFrontFace = ERHIStencilOp::Keep,
	ERHIStencilOp DepthFailOpFrontFace = ERHIStencilOp::Keep,
	ERHICompareFunc CompareFuncFrontFace = ERHICompareFunc::Always,
	uint32_t RefFrontFace = 0u,
	ERHIStencilOp FailOpBackFace = ERHIStencilOp::Keep,
	ERHIStencilOp PassOpBackFace = ERHIStencilOp::Keep,
	ERHIStencilOp DepthFailOpBackFace = ERHIStencilOp::Keep,
	ERHICompareFunc CompareFuncBackFace = ERHICompareFunc::Always,
	uint32_t RefBackFace = 0u>
class RHIDepthStencilStateStatic : public RHIStaticState<RHIDepthStencilState>
{
public:
	static std::shared_ptr<RHIDepthStencilState> Get(RHIDevice* Device)
	{
		RHIDepthStencilStateCreateInfo CreateInfo;
		CreateInfo.SetEnableDepth(EnableDepth)
			.SetEnableDepthWrite(EnableDepthWrite)
			.SetEnableDepthBoundsTest(EnableDepthBoundsTest)
			.SetEnableStencil(EnableStencil)
			.SetDepthCompareFunc(DepthCompareFunc)
			.SetStencilReadMask(StencilReadMask)
			.SetStencilWriteMask(StencilWriteMask)
			.SetDepthFailOpFrontFace(FailOpFrontFace)
			.SetStencilPassOpFrontFace(PassOpFrontFace)
			.SetDepthFailOpFrontFace(DepthFailOpFrontFace)
			.SetCompareFuncFrontFace(CompareFuncFrontFace)
			.SetRefFrontFace(RefFrontFace)
			.SetDepthFailOpBackFace(FailOpBackFace)
			.SetStencilPassOpBackFace(PassOpBackFace)
			.SetDepthFailOpBackFace(DepthFailOpBackFace)
			.SetCompareFuncBackFace(CompareFuncBackFace)
			.SetRefBackFace(RefBackFace);
	}
};

class RHIBlendStateStatic : public RHIStaticState<RHIBlendState>
{
};

