#pragma once

#include "Engine/RHI/RHIBuffer.h"
#include "Engine/RHI/RHIShader.h"
#include "Engine/RHI/RHIRenderStates.h"
#include "Engine/Asset/ShaderAsset.h"

struct RHIShaderResourceBinding
{
	uint32_t Binding = 0u;
	uint32_t DescriptorIndex = 0u;
	ERHIResourceType Type = ERHIResourceType::Unknown;

	union
	{
		RHISampler* Sampler;
		RHITexture* Texture;
		RHIBuffer* Buffer;
	};
};

using RHIShaderResourceLayout = std::array<std::vector<RHIShaderResourceBinding>, (size_t)ERHIShaderStage::Num>;

struct RHIGraphicsPipelineCreateInfo
{
	ERHIPrimitiveTopology PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;
	RHIRasterizationStateCreateInfo RasterizationState;
	RHIBlendStateCreateInfo BlendState;
	RHIDepthStencilStateCreateInfo DepthStencilState;
	RHIMultisampleStateCreateInfo MultisampleState;
	RHIInputLayoutCreateInfo InputLayout;
	std::vector<RHIViewport> Viewports;
	std::vector<RHIScissorRect> ScissorRects;
	std::array<const ShaderAsset*, (size_t)ERHIShaderStage::Num> Shaders;

	RHIRenderPassCreateInfo RenderPassCreateInfo;

	inline RHIGraphicsPipelineCreateInfo& SetPrimitiveTopology(ERHIPrimitiveTopology InTopology) { PrimitiveTopology = InTopology; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetRasterizationState(const RHIRasterizationStateCreateInfo& InRasterizationState) { RasterizationState = InRasterizationState; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetBlendState(const RHIBlendStateCreateInfo& InBlendState) { BlendState = InBlendState; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetDepthStencilState(const RHIDepthStencilStateCreateInfo& InDepthStencilState) { DepthStencilState = InDepthStencilState; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetMultisampleState(const RHIMultisampleStateCreateInfo& InMultisampleState) { MultisampleState = InMultisampleState; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetShader(const ShaderAsset* Shader) { Shaders[static_cast<size_t>(Shader->GetStage())] = Shader; return *this; }
	inline RHIGraphicsPipelineCreateInfo& SetRenderPassCreateInfo(const RHIRenderPassCreateInfo& InRenderPassCreateInfo) { RenderPassCreateInfo = InRenderPassCreateInfo; return *this; }
	
	inline RHIGraphicsPipelineCreateInfo& SetViewport(const RHIViewport& Viewport)
	{
		if (Viewports.empty())
		{
			return AddViewport(Viewport);
		}

		Viewports[0] = Viewport;
		return *this;
	}

	inline RHIGraphicsPipelineCreateInfo& AddViewport(const RHIViewport& Viewport) 
	{
		assert(Viewports.size() < ERHILimitations::MaxViewports);
		Viewports.emplace_back(Viewport);
		return *this;
	}

	inline RHIGraphicsPipelineCreateInfo& SetScissorRect(const RHIScissorRect& ScissorRect)
	{
		if (ScissorRects.empty())
		{
			return AddScissorRect(ScissorRect);
		}

		ScissorRects[0] = ScissorRect;
		return *this;
	}

	inline RHIGraphicsPipelineCreateInfo& AddScissorRect(const RHIScissorRect& ScissorRect)
	{
		assert(ScissorRects.size() < ERHILimitations::MaxScissorRects);
		ScissorRects.emplace_back(ScissorRect);
		return *this;
	}
};

struct RHIComputePipelineCreateInfo
{
};

class RHIPipelineState
{
public:
	RHIPipelineState(const RHIGraphicsPipelineCreateInfo& GfxPipelineCreateInfo);

	bool IsDirty() const { return m_PipelineStatesDirtyFlags.any() || m_ShaderResourceDirty; }

	void Commit(RHICommandBuffer* CommandBuffer);

	inline RHIPipelineState& SetBuffer(ERHIShaderStage Stage, uint32_t Binding, RHIBuffer* Buffer)
	{
		assert(Stage < ERHIShaderStage::Num);
		size_t Index = static_cast<size_t>(Stage);
		assert(Binding < m_ShaderResourceLayout[Index].size());
		MarkDirty(m_ShaderResourceLayout[Index][Binding].Buffer != Buffer);
		m_ShaderResourceLayout[Index][Binding].Buffer = Buffer;
		
		return *this;
	}

	inline RHIPipelineState& SetTexture(ERHIShaderStage Stage, uint32_t Binding, RHITexture* Texture)
	{
		assert(Stage < ERHIShaderStage::Num);
		size_t Index = static_cast<size_t>(Stage);
		assert(Binding < m_ShaderResourceLayout[Index].size());
		MarkDirty(m_ShaderResourceLayout[Index][Binding].Texture != Texture);
		m_ShaderResourceLayout[Index][Binding].Texture = Texture;
		
		return *this;
	}

	inline RHIPipelineState& SetSampler(ERHIShaderStage Stage, uint32_t Binding, RHISampler* Sampler)
	{
		assert(Stage < ERHIShaderStage::Num);
		size_t Index = static_cast<size_t>(Stage);
		assert(Binding < m_ShaderResourceLayout[Index].size());
		MarkDirty(m_ShaderResourceLayout[Index][Binding].Sampler != Sampler);
		m_ShaderResourceLayout[Index][Binding].Sampler = Sampler;

		return *this;
	}

	const RHIShaderResourceLayout& GetShaderResourceLayout() const { return m_ShaderResourceLayout; }
protected:
	enum EDirtyFlagBits
	{
		Viewport,
		ScissorRect,
		VertexStream,
		IndexBuffer,
		FrameBuffer,
		Num
	};

	void MarkDirty(bool Dirty) { m_ShaderResourceDirty |= Dirty; }
	void MarkDirty(EDirtyFlagBits Bit, bool Dirty) { m_PipelineStatesDirtyFlags.set(Bit, Dirty); }
	void ClearDirty() { m_PipelineStatesDirtyFlags.reset(); m_ShaderResourceDirty = false; }
	bool IsPipelineStatesDirty() const { return m_PipelineStatesDirtyFlags.any(); }
	bool IsShaderResourceDirty() const { return m_ShaderResourceDirty; }

	virtual void CommitShaderResources(RHICommandBuffer* CommandBuffer) = 0;
	virtual void CommitPipelineStates(RHICommandBuffer* CommandBuffer) = 0;

	bool m_ShaderResourceDirty = false;
	std::bitset<EDirtyFlagBits::Num> m_PipelineStatesDirtyFlags;
	RHIShaderResourceLayout m_ShaderResourceLayout;
};

#define ENABLE_SHADER_HOT_RELOAD 1

class RHIPipeline
{
public:
	RHIPipelineState* GetPipelineState() const { return m_PipelineState.get(); }
protected:
	std::shared_ptr<RHIPipelineState> m_PipelineState;
};

class RHIGraphicsPipeline : public RHIPipeline
{
#if ENABLE_SHADER_HOT_RELOAD
public:
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
inline size_t ComputeHash(const RHIRenderPassCreateInfo& Desc)
{
	size_t Hash = ComputeHash(
		Desc.GetNumColorAttachments(),
		Desc.HasDepthStencil(),
		Desc.SampleCount);

	for (auto& Attachment : Desc.ColorAttachments)
	{
		HashCombine(Hash, Attachment.Format);
	}
	
	HashCombine(Hash, Desc.DepthStencilAttachment.Format);

	return Hash;
}

template<>
inline size_t ComputeHash(const RHIGraphicsPipelineCreateInfo& Desc)
{
	auto Hash = ComputeHash(
		ComputeHash(Desc.PrimitiveTopology),
		ComputeHash(Desc.RasterizationState),
		ComputeHash(Desc.BlendState),
		ComputeHash(Desc.DepthStencilState),
		ComputeHash(Desc.MultisampleState),
		ComputeHash(Desc.InputLayout));

	for (auto Shader : Desc.Shaders)
	{
		if (Shader)
		{
			HashCombine(Hash, Shader);
			HashCombine(Hash, Shader->ComputeHash());
		}
	}

	return Hash;
}

template<>
inline size_t ComputeHash(const RHIFrameBufferCreateInfo& Desc)
{
	auto Hash = ComputeHash(Desc.Width, Desc.Height, Desc.ArrayLayers);

	for (auto& Color : Desc.ColorAttachments)
	{
		HashCombine(Hash, Color.Texture);
	}

	if (Desc.HasDepthStencil())
	{
		HashCombine(Hash, Desc.DepthStencilAttachment.Texture);
	}

	return Hash;
}
