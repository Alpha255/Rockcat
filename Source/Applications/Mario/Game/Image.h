#pragma once

#include "Common.h"
#include "D3DView.h"

class Image
{
public:
	enum eType
	{
		eBmp,
		ePng,
		eJpg,
		eDds,
		eTypeCount
	};

	Image() = default;
	~Image() = default;

	void Create(const char *pFileName);

	inline uint32_t Width() const 
	{
		assert(m_Width);
		return m_Width;
	}

	inline uint32_t Height() const
	{
		assert(m_Height);
		return m_Height;
	}

	inline const byte *Data() const
	{
		assert(eBmp == m_ImageType && m_pData);
		return m_pData.get();
	}

	inline const LPBITMAPINFO BitmapInfo() const
	{
		assert(eBmp == m_ImageType);
		return (LPBITMAPINFO)m_BitmapInfo.get();
	}

	inline const D3DShaderResourceView &GetShaderResourceView() const
	{
		assert(eDds == m_ImageType);
		return m_ShaderResourceView;
	}
protected:
	void CreateAsBmp(const char *pFilePath);
	void CreateAsDds(const char *pFileName);
private:
	uint32_t m_Width = 0U;
	uint32_t m_Height = 0U;
	eType m_ImageType = eTypeCount;

	std::shared_ptr<byte> m_pData = nullptr;
	std::shared_ptr<BITMAPINFO> m_BitmapInfo = nullptr;

	D3DShaderResourceView m_ShaderResourceView;
};
