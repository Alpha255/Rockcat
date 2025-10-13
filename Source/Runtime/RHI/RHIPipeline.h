#pragma once

#include "RHI/RHIBuffer.h"
#include "RHI/RHIShader.h"
#include "RHI/RHIRenderStates.h"
#include "Asset/Shader.h"

struct RHIGraphicsPipelineDesc
{
	ERHIPrimitiveTopology PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;
	RHIRasterizationStateDesc RasterizationState;
	RHIBlendStateDesc BlendState;
	RHIDepthStencilStateDesc DepthStencilState;
	RHIMultisampleStateDesc MultisampleState;
	RHIInputLayoutDesc InputLayout;
	RHIRenderPassDesc RenderPass;
	std::vector<RHIViewport> Viewports;
	std::vector<RHIScissorRect> ScissorRects;
	Array<std::shared_ptr<Shader>, ERHIShaderStage> Shaders;

	inline RHIGraphicsPipelineDesc& SetPrimitiveTopology(ERHIPrimitiveTopology InTopology) { PrimitiveTopology = InTopology; return *this; }
	inline RHIGraphicsPipelineDesc& SetRasterizationState(const RHIRasterizationStateDesc& InRasterizationState) { RasterizationState = InRasterizationState; return *this; }
	inline RHIGraphicsPipelineDesc& SetBlendState(const RHIBlendStateDesc& InBlendState) { BlendState = InBlendState; return *this; }
	inline RHIGraphicsPipelineDesc& SetDepthStencilState(const RHIDepthStencilStateDesc& InDepthStencilState) { DepthStencilState = InDepthStencilState; return *this; }
	inline RHIGraphicsPipelineDesc& SetMultisampleState(const RHIMultisampleStateDesc& InMultisampleState) { MultisampleState = InMultisampleState; return *this; }
	inline RHIGraphicsPipelineDesc& SetInputLayout(const RHIInputLayoutDesc& InInputLayout) { InputLayout = InInputLayout; return *this; }
	inline RHIGraphicsPipelineDesc& SetRenderPass(const RHIRenderPassDesc& InRenderPass) { RenderPass = InRenderPass; return *this; }
	inline RHIGraphicsPipelineDesc& SetShader(const std::shared_ptr<Shader>& InShader) { Shaders[InShader->GetStage()] = InShader; return *this; }
	
	inline RHIGraphicsPipelineDesc& SetViewport(const RHIViewport& Viewport)
	{
		if (Viewports.empty())
		{
			return AddViewport(Viewport);
		}

		Viewports[0] = Viewport;
		return *this;
	}

	inline RHIGraphicsPipelineDesc& AddViewport(const RHIViewport& Viewport) 
	{
		assert(Viewports.size() < ERHILimitations::MaxViewports);
		Viewports.emplace_back(Viewport);
		return *this;
	}

	inline RHIGraphicsPipelineDesc& SetScissorRect(const RHIScissorRect& ScissorRect)
	{
		if (ScissorRects.empty())
		{
			return AddScissorRect(ScissorRect);
		}

		ScissorRects[0] = ScissorRect;
		return *this;
	}

	inline RHIGraphicsPipelineDesc& AddScissorRect(const RHIScissorRect& ScissorRect)
	{
		assert(ScissorRects.size() < ERHILimitations::MaxScissorRects);
		ScissorRects.emplace_back(ScissorRect);
		return *this;
	}
};

struct RHIComputePipelineDesc
{
	std::shared_ptr<Shader> ComputeShader;
};

class RHIPipelineState
{
public:
	RHIPipelineState(const RHIGraphicsPipelineDesc& Desc);
	RHIPipelineState(const RHIComputePipelineDesc& Desc);
};

class RHIPipeline
{
public:
	RHIPipelineState* GetPipelineState() const { return m_PipelineState.get(); }
protected:
	std::shared_ptr<RHIPipelineState> m_PipelineState;
};

class RHIGraphicsPipeline : public RHIPipeline
{
public:
	RHIGraphicsPipeline(const RHIGraphicsPipelineDesc& Desc)
		: m_Desc(Desc)
	{
	}
private:
	RHIGraphicsPipelineDesc m_Desc;
};

class RHIComputePipeline : public RHIPipeline
{
public:
	RHIComputePipeline(const RHIComputePipelineDesc& Desc)
		: m_Desc(Desc)
	{
	}
private:
	RHIComputePipelineDesc m_Desc;
};

namespace std
{
	template<>
	struct hash<RHIRasterizationStateDesc>
	{
		size_t operator()(const RHIRasterizationStateDesc& Desc) const
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
	};

	template<>
	struct hash<RHIBlendStateDesc>
	{
		size_t operator()(const RHIBlendStateDesc& Desc) const
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
	};

	template<>
	struct hash<RHIStencilStateDesc>
	{
		size_t operator()(const RHIStencilStateDesc& Desc) const
		{
			return ComputeHash(
				Desc.FailOp,
				Desc.PassOp,
				Desc.DepthFailOp,
				Desc.CompareFunc,
				Desc.Ref);
		}
	};

	template<>
	struct hash<RHIDepthStencilStateDesc>
	{
		size_t operator()(const RHIDepthStencilStateDesc& Desc) const
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
	};

	template<>
	struct hash<RHIMultisampleStateDesc>
	{
		size_t operator()(const RHIMultisampleStateDesc& Desc) const
		{
			return ComputeHash(
				Desc.SampleCount,
				Desc.EnableSampleShading,
				Desc.EnableAlphaToCoverage,
				Desc.EnableAlphaToOne,
				Desc.MinSampleShading);
		}
	};

	template<>
	struct hash<RHIRenderPassDesc>
	{
		size_t operator()(const RHIRenderPassDesc& Desc) const
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
	};

	template<>
	struct hash<RHIFrameBufferDesc>
	{
		size_t operator()(const RHIFrameBufferDesc& Desc) const
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
	};

	template<>
	struct hash<RHIInputLayoutDesc>
	{
		size_t operator()(const RHIInputLayoutDesc& Desc) const
		{
			size_t Hash = 0u;
			for (auto& Binding : Desc.Bindings)
			{
				HashCombine(Hash, Binding.Binding, Binding.Stride, Binding.InputRate);
				for (auto& Attribute : Binding.Attributes)
				{
					HashCombine(Hash, Attribute.Location, Attribute.Stride, Attribute.Format);
					HashCombine(Hash, std::hash<std::string>()(Attribute.Usage));
				}
			}
			return Hash;
		}
	};

	template<>
	struct hash<RHIGraphicsPipelineDesc>
	{
		size_t operator()(const RHIGraphicsPipelineDesc& Desc) const
		{
			auto Hash = ComputeHash(
				ComputeHash(Desc.PrimitiveTopology),
				ComputeHash(Desc.RasterizationState),
				ComputeHash(Desc.BlendState),
				ComputeHash(Desc.DepthStencilState),
				ComputeHash(Desc.MultisampleState),
				ComputeHash(Desc.InputLayout));

			for (auto& Shader : Desc.Shaders)
			{
				if (Shader)
				{
					HashCombine(Hash, Shader->GetHash());
				}
			}

			return Hash;
		}
	};
}
