#pragma once

#include "Asset/Asset.h"
#include "Rendering/RHI/RHITexture.h"

class TextureAsset : public Asset
{
public:
	using Asset::Asset;

	inline uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() const { return m_Height; }
	inline uint32_t GetDepth() const { return m_Depth; }
	inline uint32_t GetNumArrayLayers() const { return m_NumArrayLayers; }
	inline uint32_t GetNumMips() const { return m_NumMips; }
	inline ERHITextureDimension GetDimension() const { return m_Dimension; }
	inline ERHIFormat GetFormat() const { return m_Format; }
	inline const DataBlock& GetBulkData() const { return m_BulkData; }
private:
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_Depth;
	uint32_t m_NumArrayLayers;
	uint32_t m_NumMips;
	ERHITextureDimension m_Dimension;
	ERHIFormat m_Format;
	DataBlock m_BulkData;
};
