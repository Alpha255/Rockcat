#pragma once

#include "Runtime/Engine/RHI/RHIResource.h"
#include "Runtime/Core/Math/Vector4.h"

enum class ERHIPolygonMode : uint8_t
{
	Solid,
	Wireframe
};

enum class ERHICullMode : uint8_t
{
	None,
	FrontFace,
	BackFace,
	FrontAndBackFace
};

enum class ERHIFrontFace : uint8_t
{
	Counterclockwise,
	Clockwise
};

enum class ERHIBlendFactor : uint8_t
{
	Zero,
	One,
	Constant,
	InverseConstant,

	SrcAlpha,
	InverseSrcAlpha,
	DstAlpha,
	InverseDstAlpha,
	SrcAlphaSaturate,
	Src1Alpha,
	InverseSrc1Alpha,

	SrcColor,
	InverseSrcColor,
	DstColor,
	InverseDstColor,
	Src1Color,
	InverseSrc1Color
};

enum class ERHIBlendOp : uint8_t
{
	Add,
	Subtract,
	ReverseSubtract,
	Min,
	Max
};

enum class ERHIColorWriteMask : uint8_t
{
	None,
	Red = 1 << 0,
	Green = 1 << 1,
	Blue = 1 << 2,
	Alpha = 1 << 3,
	All = Red | Green | Blue | Alpha
};
ENUM_FLAG_OPERATORS(ERHIColorWriteMask)

enum class ERHILogicOp : uint8_t
{
	Clear,
	And,
	And_Reverse,
	Copy,
	And_Inverted,
	No,
	Xor,
	Or,
	Nor,
	Equivalent,
	Invert,
	Or_Reverse,
	Copy_Inverted,
	Or_Inverted,
	Nand,
	Set
};

enum class ERHICompareFunc : uint8_t
{
	Always,
	Never,
	Less,
	LessOrEqual,
	Equal,
	NotEqual,
	Greater,
	GreaterOrEqual
};

enum class ERHIStencilOp : uint8_t
{
	Keep,
	Zero,
	Replace,                 /// Set the stencil data to the reference value
	IncrementAndClamp,       /// Increment the stencil value by 1, and clamp the result
	DecrementAndClamp,       /// Decrement the stencil value by 1, and clamp the result
	Invert,
	IncrementAndWrap,        /// Increment the stencil value by 1, and wrap the result if necessary
	DecrementAndWrap         /// Increment the stencil value by 1, and wrap the result if necessary
};

enum class ERHISampleCount : uint8_t
{
	Sample_1_Bit = 1 << 0,
	Sample_2_Bit = 1 << 1,
	Sample_4_Bit = 1 << 2,
	Sample_8_Bit = 1 << 3,
	Sample_16_Bit = 1 << 4,
	Sample_32_Bit = 1 << 5,
	Sample_64_Bit = 1 << 6
};

enum class ERHIPrimitiveTopology : uint8_t
{
	PointList,
	LineList,
	LineStrip,
	TriangleList,
	TriangleStrip,
	LineListAdjacency,
	LineStripAdjacency,
	TriangleListAdjacency,
	TriangleStripAdjacency,
	PatchList
};

struct RHIRenderTargetBlendDesc
{
	uint8_t Index = ERHILimitations::MaxRenderTargets;

	bool8_t Enable = false;
	ERHIColorWriteMask ColorMask = ERHIColorWriteMask::All;

	ERHIBlendFactor SrcColor = ERHIBlendFactor::One;
	ERHIBlendFactor DstColor = ERHIBlendFactor::Zero;
	ERHIBlendOp ColorOp = ERHIBlendOp::Add;

	ERHIBlendFactor SrcAlpha = ERHIBlendFactor::One;
	ERHIBlendFactor DstAlpha = ERHIBlendFactor::Zero;
	ERHIBlendOp AlphaOp = ERHIBlendOp::Add;

	inline RHIRenderTargetBlendDesc& SetIndex(uint8_t Value) { Index = Value; return *this; }
	inline RHIRenderTargetBlendDesc& SetEnable(bool8_t IsEnable) { Enable = IsEnable; return *this; }
	inline RHIRenderTargetBlendDesc& SetColorMask(ERHIColorWriteMask ColorWriteMask) { ColorMask = ColorWriteMask; return *this; }
	inline RHIRenderTargetBlendDesc& SetSrcColorBlendFactor(ERHIBlendFactor SrcColorBlendFactor) { SrcColor = SrcColorBlendFactor; return *this; }
	inline RHIRenderTargetBlendDesc& SetDstColorBlendFactor(ERHIBlendFactor DstColorBlendFactor) { DstColor = DstColorBlendFactor; return *this; }
	inline RHIRenderTargetBlendDesc& SetColorBlendOp(ERHIBlendOp ColorBlendOp) { ColorOp = ColorBlendOp; return *this; }
	inline RHIRenderTargetBlendDesc& SetSrcAlphaBlendFactor(ERHIBlendFactor SrcAlphaBlendFactor) { SrcAlpha = SrcAlphaBlendFactor; return *this; }
	inline RHIRenderTargetBlendDesc& SetDstAlphaBlendFactor(ERHIBlendFactor DstAlphaBlendFactor) { DstAlpha = DstAlphaBlendFactor; return *this; }
	inline RHIRenderTargetBlendDesc& SetAlphaBlendOp(ERHIBlendOp AlphaBlendOp) { AlphaOp = AlphaBlendOp; return *this; }

	inline bool8_t IsValid() const { return Index != ERHILimitations::MaxRenderTargets; }
};

/// Move AlphaToCoverage/IndependentBlend to MultisampleState
struct RHIBlendStateCreateInfo : public RHIHashedObject
{
	bool8_t EnableLogicOp = false;
	ERHILogicOp LogicOp = ERHILogicOp::No;

	RHIRenderTargetBlendDesc RenderTargetBlends[ERHILimitations::MaxRenderTargets];

	inline RHIBlendStateCreateInfo& SetEnableLogicOp(bool8_t Enable) { EnableLogicOp = Enable; return *this; }
	inline RHIBlendStateCreateInfo& SetLogicOp(ERHILogicOp Op) { LogicOp = Op; return *this; }

	size_t GetHash() const override final
	{
		if (HashValue == InvalidHash)
		{
			HashValue = ComputeHash(EnableLogicOp, LogicOp);
			for (uint32_t Index = 0u; Index < ERHILimitations::MaxRenderTargets; ++Index)
			{
				if (RenderTargetBlends[Index].Index != ERHILimitations::MaxRenderTargets)
				{
					HashCombine(HashValue, ComputeHash(
						RenderTargetBlends[Index].Index,
						RenderTargetBlends[Index].Enable,
						RenderTargetBlends[Index].ColorMask,
						RenderTargetBlends[Index].SrcColor,
						RenderTargetBlends[Index].DstColor,
						RenderTargetBlends[Index].ColorOp,
						RenderTargetBlends[Index].SrcAlpha,
						RenderTargetBlends[Index].DstAlpha,
						RenderTargetBlends[Index].AlphaOp));
				}
			}
		}
		return HashValue;
	}
};

struct RHIRasterizationStateDesc : public RHIHashedObject
{
	ERHIPolygonMode PolygonMode = ERHIPolygonMode::Solid;
	ERHICullMode CullMode = ERHICullMode::BackFace;
	ERHIFrontFace FrontFace = ERHIFrontFace::Clockwise;
	bool8_t EnableDepthClamp = false;

	float32_t DepthBias = 0.0f;       /// A scalar factor controlling the constant depth value added to each fragment.
	float32_t DepthBiasClamp = 0.0f;  /// The maximum (or minimum) depth bias of a fragment.
	float32_t DepthBiasSlope = 0.0f;  /// A scalar factor applied to a fragment¡¯s slope in depth bias calculations.
	float32_t LineWidth = 1.0f;

	inline RHIRasterizationStateDesc& SetPolygonMode(ERHIPolygonMode Mode) { PolygonMode = Mode; return *this; }
	inline RHIRasterizationStateDesc& SetCullMode(ERHICullMode Mode) { CullMode = Mode; return *this; }
	inline RHIRasterizationStateDesc& SetFrontFace(ERHIFrontFace Face) { FrontFace = Face; return *this; }
	inline RHIRasterizationStateDesc& SetEnableDepthClamp(bool8_t Enable) { EnableDepthClamp = Enable; return *this; }
	inline RHIRasterizationStateDesc& SetDepthBias(float32_t Bias) { DepthBias = Bias; return *this; }
	inline RHIRasterizationStateDesc& SetDepthBiasClamp(float32_t BiasClamp) { DepthBiasClamp = BiasClamp; return *this; }
	inline RHIRasterizationStateDesc& SetDepthBiasSlope(float32_t BiasSlope) { DepthBiasSlope = BiasSlope; return *this; }
	inline RHIRasterizationStateDesc& SetLineWidth(float32_t Width) { LineWidth = Width; return *this; }

	size_t GetHash() const override final
	{
		if (HashValue == InvalidHash)
		{
			HashValue = ComputeHash(
				PolygonMode,
				CullMode,
				FrontFace,
				EnableDepthClamp,
				DepthBias,
				DepthBiasClamp,
				DepthBiasSlope,
				LineWidth);
		}
		return HashValue;
	}
};

struct RHIStencilStateDesc
{
	ERHIStencilOp FailOp = ERHIStencilOp::Keep;
	ERHIStencilOp PassOp = ERHIStencilOp::Keep;
	ERHIStencilOp DepthFailOp = ERHIStencilOp::Keep;
	ERHICompareFunc CompareFunc = ERHICompareFunc::Always;

	uint32_t Ref = 0u;
};

struct RHIDepthStencilStateDesc : public RHIHashedObject
{
	bool8_t EnableDepth = true;
	bool8_t EnableDepthWrite = false;
	bool8_t EnableDepthBoundsTest = false;
	bool8_t EnableStencil = false;

	ERHICompareFunc DepthCompareFunc = ERHICompareFunc::Less;
	uint8_t StencilReadMask = 0xFF;
	uint8_t StencilWriteMask = 0xFF;

	Math::Vector2 DepthBounds;

	RHIStencilStateDesc FrontFaceStencil;
	RHIStencilStateDesc BackFaceStencil;

	inline RHIDepthStencilStateDesc& SetEnableDepth(bool8_t Enable) { EnableDepth = Enable; return *this; }
	inline RHIDepthStencilStateDesc& SetEnableDepthWrite(bool8_t Enable) { EnableDepthWrite = Enable; return *this; }
	inline RHIDepthStencilStateDesc& SetEnableDepthBoundsTest(bool8_t Enable) { EnableDepthBoundsTest = Enable; return *this; }
	inline RHIDepthStencilStateDesc& SetEnableStencil(bool8_t Enable) { EnableStencil = Enable; return *this; }
	inline RHIDepthStencilStateDesc& SetDepthCompareFunc(ERHICompareFunc CompareFunc) { DepthCompareFunc = CompareFunc; return *this; }
	inline RHIDepthStencilStateDesc& SetStencilReadMask(uint8_t Mask) { StencilReadMask = Mask; return *this; }
	inline RHIDepthStencilStateDesc& SetStencilWriteMask(uint8_t Mask) { StencilWriteMask = Mask; return *this; }
	inline RHIDepthStencilStateDesc& SetDepthBounds(const Math::Vector2& Bounds) { DepthBounds = Bounds; return *this; }
	inline RHIDepthStencilStateDesc& SetStencilFailOpFrontFace(ERHIStencilOp Op) { FrontFaceStencil.FailOp = Op; return *this; }
	inline RHIDepthStencilStateDesc& SetStencilPassOpFrontFace(ERHIStencilOp Op) { FrontFaceStencil.PassOp = Op; return *this; }
	inline RHIDepthStencilStateDesc& SetDepthFailOpFrontFace(ERHIStencilOp Op) { FrontFaceStencil.DepthFailOp = Op; return *this; }
	inline RHIDepthStencilStateDesc& SetCompareFuncFrontFace(ERHICompareFunc Func) { FrontFaceStencil.CompareFunc = Func; return *this; }
	inline RHIDepthStencilStateDesc& SetRefFrontFace(uint32_t Ref) { FrontFaceStencil.Ref = Ref; return *this; }
	inline RHIDepthStencilStateDesc& SetStencilFailOpBackFace(ERHIStencilOp Op) { BackFaceStencil.FailOp = Op; return *this; }
	inline RHIDepthStencilStateDesc& SetStencilPassOpBackFace(ERHIStencilOp Op) { BackFaceStencil.PassOp = Op; return *this; }
	inline RHIDepthStencilStateDesc& SetDepthFailOpBackFace(ERHIStencilOp Op) { BackFaceStencil.DepthFailOp = Op; return *this; }
	inline RHIDepthStencilStateDesc& SetCompareFuncBackFace(ERHICompareFunc Func) { BackFaceStencil.CompareFunc = Func; return *this; }
	inline RHIDepthStencilStateDesc& SetRefBackFace(uint32_t Ref) { BackFaceStencil.Ref = Ref; return *this; }

	size_t GetHash() const override final
	{
		if (HashValue == InvalidHash)
		{
			HashValue = ComputeHash(
				EnableDepth,
				EnableDepthWrite,
				EnableDepthBoundsTest,
				EnableStencil,
				DepthCompareFunc,
				StencilReadMask,
				StencilWriteMask);

			HashCombine(HashValue, ComputeHash(
				FrontFaceStencil.FailOp,
				FrontFaceStencil.PassOp,
				FrontFaceStencil.DepthFailOp,
				FrontFaceStencil.CompareFunc,
				FrontFaceStencil.Ref));

			HashCombine(HashValue, ComputeHash(
				BackFaceStencil.FailOp,
				BackFaceStencil.PassOp,
				BackFaceStencil.DepthFailOp,
				BackFaceStencil.CompareFunc,
				BackFaceStencil.Ref));
		}
		return HashValue;
	}
};

struct RHIMultisampleStateDesc : public RHIHashedObject
{
	ERHISampleCount SampleCount = ERHISampleCount::Sample_1_Bit;
	bool8_t EnableSampleShading = false;
	bool8_t EnableAlphaToCoverage = false;
	bool8_t EnableAlphaToOne = false;

	float32_t MinSampleShading = 0.0f;
	uint32_t* SampleMask = nullptr; /// ??? 

	inline RHIMultisampleStateDesc& SetSampleCount(ERHISampleCount Count) { SampleCount = Count; return *this; }
	inline RHIMultisampleStateDesc& SetEnableSampleShading(bool8_t Enable) { EnableSampleShading = Enable; return *this; }
	inline RHIMultisampleStateDesc& SetEnableAlphaToCoverage(bool8_t Enable) { EnableAlphaToCoverage = Enable; return *this; }
	inline RHIMultisampleStateDesc& SetEnableAlphaToOne(bool8_t Enable) { EnableAlphaToOne = Enable; return *this; }
	inline RHIMultisampleStateDesc& SetMinSampleShading(float32_t SampleShading) { MinSampleShading = SampleShading; return *this; }
	inline RHIMultisampleStateDesc& SetSampleMask(uint32_t* Mask) { SampleMask = Mask; return *this; }

	size_t GetHash() const override final
	{
		if (HashValue == InvalidHash)
		{
			HashValue = ComputeHash(
				SampleCount,
				EnableSampleShading,
				EnableAlphaToCoverage,
				EnableAlphaToOne,
				MinSampleShading,
				SampleMask);
		}
		return HashValue;
	}
};

struct RHIViewport
{
	Math::Vector2 LeftTop;
	Math::Vector2 Extent;
	Math::Vector2 DepthRange;

	RHIViewport()
		: LeftTop(0.0f, 0.0f)
		, Extent(0.0f, 0.0f)
		, DepthRange(0.0f, 1.0f)
	{
	}

	RHIViewport(float32_t Width, float32_t Height)
		: LeftTop(0.0f, 0.0f)
		, Extent(Width, Height)
		, DepthRange(0.0f, 1.0f)
	{
	}

	RHIViewport(float32_t Left, float32_t Top, float32_t Width, float32_t Height, float32_t MinZ = 0.0f, float32_t MaxZ = 1.0f)
		: LeftTop(Left, Top)
		, Extent(Width, Height)
		, DepthRange(MinZ, MaxZ)
	{
	}

	float32_t GetWidth() const { return Extent.x; }
	float32_t GetHeight() const { return Extent.y; }

	bool8_t operator==(const RHIViewport& Other) const { return LeftTop == Other.LeftTop && Extent == Other.Extent && DepthRange == Other.DepthRange; }
	bool8_t operator!=(const RHIViewport& Other) const { return LeftTop != Other.LeftTop || Extent != Other.Extent || DepthRange != Other.DepthRange; }
};

struct RHIScissorRect
{
	Math::Vector2 LeftTop;
	Math::Vector2 Extent;

	RHIScissorRect()
		: LeftTop(0.0f, 0.0f)
		, Extent(0.0f, 0.0f)
	{
	}

	RHIScissorRect(float32_t Width, float32_t Height)
		: LeftTop(0.0f, 0.0f)
		, Extent(Width, Height)
	{
		assert(Width > 0.0f && Height > 0.0f);
	}

	RHIScissorRect(float32_t Left, float32_t Top, float32_t Width, float32_t Height)
		: LeftTop(Left, Top)
		, Extent(Width, Height)
	{
		assert(Width > 0.0f && Height > 0.0f);
	}

	bool8_t operator==(const RHIScissorRect& Other) const { return LeftTop == Other.LeftTop && Extent == Other.Extent; }
	bool8_t operator!=(const RHIScissorRect& Other) const { return LeftTop != Other.LeftTop || Extent != Other.Extent; }
};

enum class ERHIAttachmentLoadOp
{
	Load,
	Clear,
	DontCare
};

enum class ERHIAttachmentStoreOp
{
	Store,
	DontCare
};

struct RHIAttachmentReference
{
	uint32_t Index = ~0u; /// VK_ATTACHMENT_UNUSED
	ERHIResourceState State = ERHIResourceState::Unknown;
};

struct RHIAttachmentDesc
{
	ERHIFormat Format = ERHIFormat::Unknown;
	uint8_t SampleCount = 1u;
	ERHIAttachmentLoadOp LoadOp = ERHIAttachmentLoadOp::Clear;
	ERHIAttachmentStoreOp StoreOp = ERHIAttachmentStoreOp::DontCare;
	ERHIAttachmentLoadOp StencilLoadOp = ERHIAttachmentLoadOp::Clear;
	ERHIAttachmentStoreOp StencilStoreOp = ERHIAttachmentStoreOp::DontCare;
	ERHIResourceState InitialState = ERHIResourceState::Unknown;
	ERHIResourceState FinalState = ERHIResourceState::Unknown;
};

struct RHISubPassDependency
{
	uint32_t SrcSubpassIndex = ~0u;  /// VK_SUBPASS_EXTERNAL
	uint32_t DstSubpassIndex = ~0u;
	//GfxFlags SrcStageFlags = 0u;
	//GfxFlags DstStageFlags = 0u;
	//GfxFlags SrcAccessFlags = 0u;
	//GfxFlags DstAccessFlags = 0u;
};

struct RHISubPassDesc
{
	std::vector<RHIAttachmentReference> InputAttachments;
	std::vector<RHIAttachmentReference> ColorAttachments;
	std::vector<RHIAttachmentReference> ResolveAttachments;
	std::vector<uint32_t> PreserveAttachments;
	RHIAttachmentReference DepthStencilAttachment;
};

struct RHIRenderPassDesc : public RHIHashedObject
{
	std::vector<RHIAttachmentDesc> AttachmentDescs;
	std::vector<RHISubPassDesc> SubPasses;
	std::vector<RHISubPassDependency> SubPassDependencies;

	size_t GetHash() const override final
	{
		assert(0);
		if (HashValue == InvalidHash)
		{
			HashValue = ComputeHash(AttachmentDescs.size());
			for (auto& AttachmentDesc : AttachmentDescs)
			{
				HashCombine(HashValue, ComputeHash(
					AttachmentDesc.Format,
					AttachmentDesc.SampleCount,
					AttachmentDesc.LoadOp,
					AttachmentDesc.StoreOp,
					AttachmentDesc.StencilLoadOp,
					AttachmentDesc.StencilStoreOp,
					AttachmentDesc.InitialState,
					AttachmentDesc.FinalState));
			}
		}
		return HashValue;
	}
};
