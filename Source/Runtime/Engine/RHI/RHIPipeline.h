#pragma once

#include "Engine/RHI/RHIBuffer.h"
#include "Engine/RHI/RHIShader.h"
#include "Engine/RHI/RHIRenderStates.h"

struct RHIShaderVariableBinding
{
	ERHIShaderStage Stage = ERHIShaderStage::Num;
	uint32_t Binding = 0u;
	ERHIResourceType Type = ERHIResourceType::Unknown;
};
using RHIPipelineLayoutDesc = std::vector<RHIShaderVariableBinding>;

struct RHIGraphicsPipelineCreateInfo
{
	ERHIPrimitiveTopology PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;
	RHIRasterizationStateCreateInfo RasterizationState;
	RHIBlendStateCreateInfo BlendState;
	RHIDepthStencilStateCreateInfo DepthStencilState;
	RHIMultisampleStateCreateInfo MultisampleState;
	std::array<RHIShaderCreateInfo, (size_t)ERHIShaderStage::Num> Shaders;

	inline RHIGraphicsPipelineCreateInfo& SetPrimitiveTopology(ERHIPrimitiveTopology InTopology) { PrimitiveTopology = InTopology; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetRasterizationState(const RHIRasterizationStateCreateInfo& InRasterizationState) { RasterizationState = InRasterizationState; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetBlendState(const RHIBlendStateCreateInfo& InBlendState) { BlendState = InBlendState; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetDepthStencilState(const RHIDepthStencilStateCreateInfo& InDepthStencilState) { DepthStencilState = InDepthStencilState; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetMultisampleState(const RHIMultisampleStateCreateInfo& InMultisampleState) { MultisampleState = InMultisampleState; return *this; }
	RHIGraphicsPipelineCreateInfo& SetShader(const class ShaderAsset& Shader);
};

struct RHIComputePipelineCreateInfo
{
};

struct RHIPipelineState
{
#if 0
public:
	enum class EDirtyFlags : uint8_t
	{
		Resources,
		VertexBuffer,
		IndexBuffer,
		Viewport,
		ScissorRect,
		PolygonMode,
		Max
	};

	PipelineState(const GraphicsPipelineDesc& Desc);

	virtual ~PipelineState() = default;

	template<EShaderStage Stage>
	void SetImage(uint32_t Binding, const IImageSharedPtr& Image)
	{
		if (auto Variable = ShaderVariableTable.Find(Stage, Binding))
		{
			if (Variable->Get() != Image.get())
			{
				assert(Variable->ShaderStage() == Stage);
				assert(Variable->ResourceType() == EResourceType::SampledImage || Variable->ResourceType() == EResourceType::StorageImage);

				SetDirty<EDirtyFlags::Resources>(true);
				Variable->Set(Image);
				WriteImage(Stage, Binding, Image.get());
			}
		}
	}

	template<EShaderStage Stage>
	void SetSampler(uint32_t Binding, const ISamplerSharedPtr& Sampler)
	{
		if (auto Variable = ShaderVariableTable.Find(Stage, Binding))
		{
			if (Variable->Get() != Sampler.get())
			{
				assert(Variable->ShaderStage() == Stage);
				assert(Variable->ResourceType() == EResourceType::Sampler);

				SetDirty<EDirtyFlags::Resources>(true);
				Variable->Set(Sampler);
				WriteSampler(Stage, Binding, Sampler.get());
			}
		}
	}

	template<EShaderStage Stage>
	void SetUniformBuffer(uint32_t Binding, const IBufferSharedPtr& Buffer)
	{
		if (auto Variable = ShaderVariableTable.Find(Stage, Binding))
		{
			if (Variable->Get() != Buffer.get())
			{
				assert(Variable->ShaderStage() == Stage);
				assert(Variable->ResourceType() == EResourceType::UniformBuffer);

				SetDirty<EDirtyFlags::Resources>(true);
				Variable->Set(Buffer);
				WriteUniformBuffer(Stage, Binding, Buffer.get());
			}
		}
	}

	void SetVertexBuffer(IBuffer* Buffer)
	{
		if (VertexBuffer != Buffer)
		{
			SetDirty<EDirtyFlags::VertexBuffer>(true);
			VertexBuffer = Buffer;
		}
	}

	void SetIndexBuffer(IBuffer* Buffer, EIndexFormat Format)
	{
		if (IndexBuffer.first != Buffer)
		{
			SetDirty<EDirtyFlags::IndexBuffer>(true);
			IndexBuffer = std::make_pair(Buffer, Format);
		}
	}

	void SetPolygonMode(EPolygonMode Mode)
	{
		if (PolygonMode != Mode)
		{
			SetDirty<EDirtyFlags::PolygonMode>(true);
			PolygonMode = Mode;
		}
	}

	void SetViewport(const Viewport& InViewport)
	{
		if (Viewport != InViewport)
		{
			SetDirty<EDirtyFlags::Viewport>(true);
			Viewport = InViewport;
		}
	}

	void SetScissorRect(const ScissorRect& Rect)
	{
		if (Scissor != Rect)
		{
			SetDirty<EDirtyFlags::ScissorRect>(true);
			Scissor = Rect;
		}
	}

	const PipelineShaderVariableTable& GetShaderVariableTable() const
	{
		return ShaderVariableTable;
	}

	template<EDirtyFlags Flags>
	const bool IsDirty() const
	{
		return Dirty[static_cast<size_t>(Flags)];
	}

	template<EDirtyFlags Flags>
	void SetDirty(bool IsDirty)
	{
		Dirty.set(static_cast<size_t>(Flags), IsDirty);
	}

	void Reset()
	{
		Dirty.reset();
	}
protected:
	virtual void WriteImage(EShaderStage Stage, uint32_t Binding, IImage* Image) = 0;
	virtual void WriteSampler(EShaderStage Stage, uint32_t Binding, ISampler* Sampler) = 0;
	virtual void WriteUniformBuffer(EShaderStage Stage, uint32_t Binding, IBuffer* Buffer) = 0;
public:
	IBuffer* VertexBuffer = nullptr;
	std::pair<IBuffer*, EIndexFormat> IndexBuffer;
	Viewport Viewport;
	ScissorRect Scissor;
	EPolygonMode PolygonMode = EPolygonMode::Solid;
	IFrameBuffer* FrameBuffer = nullptr;
private:
	std::bitset<static_cast<size_t>(EDirtyFlags::Max)> Dirty;
	PipelineShaderVariableTable ShaderVariableTable;
#endif
};

class RHIDescriptorSet
{
public:
	virtual void Commit(const RHIPipelineLayoutDesc& Desc) = 0;
};

#define ENABLE_SHADER_HOT_RELOAD 1

class RHIPipeline
{
};

class RHIGraphicsPipeline : public RHIPipeline
{
#if ENABLE_SHADER_HOT_RELOAD
	RHIGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& CreateInfo)
		: m_CreateInfo(CreateInfo)
	{
	}
private:
	RHIGraphicsPipelineCreateInfo m_CreateInfo;
#endif
};

class RHIComputePipeline : public RHIPipeline
{
};

template<class RHIDescription>
inline size_t ComputeHash(const RHIDescription&) { return 0ull; }

template<>
inline size_t ComputeHash(const RHIRasterizationStateCreateInfo& Desc)
{
	return ComputeHash(
		Desc.PolygonMode,
		Desc.CullMode,
		Desc.FrontFace,
		Desc.EnableDepthClamp,
		Desc.DepthBias,
		Desc.DepthBiasClamp,
		Desc.DepthBiasSlope,
		Desc.LineWidth);
}

template<>
inline size_t ComputeHash(const RHIBlendStateCreateInfo& Desc)
{
	auto Hash = ComputeHash(
		Desc.EnableLogicOp,
		Desc.LogicOp);
	for (uint32_t Index = 0u; Index < ERHILimitations::MaxRenderTargets; ++Index)
	{
		auto& BlendState = Desc.RenderTargetBlends[Index];
		HashCombine(Hash, 
			BlendState.Enable,
			BlendState.ColorMask,
			BlendState.SrcColor,
			BlendState.DstColor,
			BlendState.ColorOp,
			BlendState.SrcAlpha,
			BlendState.DstAlpha,
			BlendState.AlphaOp);
	}
	return Hash;
}

template<>
inline size_t ComputeHash(const RHIStencilStateDesc& Desc)
{
	return ComputeHash(
		Desc.FailOp,
		Desc.PassOp,
		Desc.DepthFailOp,
		Desc.CompareFunc,
		Desc.Ref);
}

template<>
inline size_t ComputeHash(const RHIDepthStencilStateCreateInfo& Desc)
{
	return ComputeHash(
		Desc.EnableDepth,
		Desc.EnableDepthWrite,
		Desc.EnableDepthBoundsTest,
		Desc.EnableStencil,
		Desc.DepthCompareFunc,
		Desc.StencilReadMask,
		Desc.StencilWriteMask,
		Desc.DepthBounds.x,
		Desc.DepthBounds.y,
		ComputeHash(Desc.FrontFaceStencil),
		ComputeHash(Desc.BackFaceStencil));
}

template<>
inline size_t ComputeHash(const RHIMultisampleStateCreateInfo& Desc)
{
	return ComputeHash(
		Desc.SampleCount,
		Desc.EnableSampleShading,
		Desc.EnableAlphaToCoverage,
		Desc.EnableAlphaToOne,
		Desc.MinSampleShading);
}

template<>
inline size_t ComputeHash(const RHIGraphicsPipelineCreateInfo& Desc)
{
	auto Hash = ComputeHash(
		ComputeHash(Desc.PrimitiveTopology),
		ComputeHash(Desc.RasterizationState),
		ComputeHash(Desc.BlendState),
		ComputeHash(Desc.DepthStencilState),
		ComputeHash(Desc.MultisampleState));
	for (uint32_t Index = 0u; Index < Desc.Shaders.size(); ++Index)
	{
		HashCombine(Hash, Desc.Shaders[Index].Stage, Desc.Shaders[Index].Name, Desc.Shaders[Index].Binary);
	}
	return Hash;
}
