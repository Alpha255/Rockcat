#pragma once

#include "Core/Math/Color.h"
#include "Asset/Asset.h"
#include "Asset/RenderResource.h"
#include "RHI/RHITexture.h"

class Texture : public Asset, public RenderResource
{
public:
	using Asset::Asset;

	const RHITexture* GetRHI() const { assert(m_RHIResource); return m_RHIResource.get(); }

	inline uint32_t GetWidth() const { return m_Desc.Width; }
	inline uint32_t GetHeight() const { return m_Desc.Height; }
	inline uint32_t GetDepth() const { return m_Desc.Depth; }
	inline uint32_t GetNumArrayLayers() const { return m_Desc.NumArrayLayer; }
	inline uint32_t GetNumMipLevels() const { return m_Desc.NumMipLevel; }
	inline ERHITextureDimension GetDimension() const { return m_Desc.Dimension; }
	inline ERHIFormat GetFormat() const { return m_Desc.Format; }
	inline ERHISampleCount GetSampleCount() const { return m_Desc.SampleCount; }
	inline bool IsLinear() const { return m_Desc.IsLinear; }
	inline const DataBlock& GetBulkData() const { return *m_Desc.BulkData; }

protected:
	friend class TextureLoader;

	inline RHITextureDesc& GetDesc() { return m_Desc; }

	static Texture* CreateColoredTexture(std::string_view Name, const Math::Color& Value, uint32_t Width = 1u, uint32_t Height = 1u);

	void CreateRHI();

	RHITextureDesc m_Desc;
	RHITexturePtr m_RHIResource;
};
