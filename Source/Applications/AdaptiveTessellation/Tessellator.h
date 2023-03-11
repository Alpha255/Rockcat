#pragma once

#include "Scanner.h"

class Tessellator
{
public:
	enum ePartitioningMode
	{
		eInteger,
		ePow2,
		eFractionalOdd,
		eFractionalEven,
		ePartitioningModeCount
	};

	Tessellator() = default;
	~Tessellator() = default;

	inline ePartitioningMode GetPartitioningMode() const
	{
		return m_ptMode;
	}
	inline void SetPartitioningMode(ePartitioningMode mode)
	{
		m_ptMode = mode;
	}

	inline void Resize(uint32_t width, uint32_t height)
	{
		const float adaptiveScaleInPixels = 15.0f;
		m_EdgeLenScale.x = width * 0.5f / adaptiveScaleInPixels;
		m_EdgeLenScale.y = height * 0.5f / adaptiveScaleInPixels;
	}

	inline void Init(uint32_t vertexCount, ID3D11Buffer *pVertexBuf)
	{
		m_SrcVertexCount = vertexCount;

		CreateLookupTable();
		CreateResource(vertexCount, pVertexBuf);

		m_Scanner.Init(vertexCount);
	}

	inline uint32_t GetTessedVertexCount()
	{
		return m_CachedTessedVertexCount;
	}

	inline uint32_t GetTessedIndexCount()
	{
		return m_CachedTessedIndexCount;
	}

	inline const Ref<ID3D11ShaderResourceView> &GetSourceVertexBufferSRV() const 
	{
		return m_Res.SRV_VB_Src;
	}

	inline const Ref<ID3D11ShaderResourceView> &GetTessedVertexBufferSRV() const 
	{
		return m_Res.SRV_VB_Tessed;
	}

	inline const Ref<ID3D11Buffer> &GetTessedIndexBuffer() const
	{
		return m_Res.IB_Tessed;
	}

	bool DoTessellationByEdge(const class Camera &cam);
protected:
	void CreateLookupTable();
	void CreateResource(uint32_t vertexCount, ID3D11Buffer *pVertexBuf);

	void ExeComputeShader(
		const Ref<ID3D11ComputeShader> &rTargetCS, 
		uint32_t srvCount, 
		ID3D11ShaderResourceView **ppSRVs, 
		const Ref<ID3D11Buffer> &rCBConstant, 
		const Ref<ID3D11Buffer> &rCBUpdate, 
		uint32_t cbufSize, 
		const void *pBufData,
		const Ref<ID3D11UnorderedAccessView> &rUAV,
		uint32_t patchX, 
		uint32_t patchY, 
		uint32_t patchZ);
private:
	ePartitioningMode m_ptMode = eFractionalEven;
	Vec2 m_EdgeLenScale = Vec2(0.0f, 0.0f);
	static const uint32_t s_MaxFactor = 16U;

	int32_t m_InsidePointIndex[s_MaxFactor / 2 + 1][s_MaxFactor / 2 + 2][4] = { 0 };
	int32_t m_OutsidePointIndex[s_MaxFactor / 2 + 1][s_MaxFactor / 2 + 2][4] = { 0 };

	struct D3DResource
	{
		Ref<ID3D11ComputeShader> CS_EdgeFactor;
		Ref<ID3D11ComputeShader> CS_NumVI;
		Ref<ID3D11ComputeShader> CS_ScatterVertexTriID_IndexID;
		Ref<ID3D11ComputeShader> CS_ScatterIndexTriID_IndexID;
		Ref<ID3D11ComputeShader> CS_TessVertex;
		Ref<ID3D11ComputeShader> CS_TessIndex;

		Ref<ID3D11Buffer> CB_EdgeFactor;
		Ref<ID3D11Buffer> CB_LookupTable;
		Ref<ID3D11Buffer> CB_Tess;
		Ref<ID3D11Buffer> CB_ReadBackBuf;

		Ref<ID3D11Buffer> VB_Scatter;
		Ref<ID3D11Buffer> IB_Scatter;

		Ref<ID3D11Buffer> VB_Tessed;
		Ref<ID3D11Buffer> IB_Tessed;

		Ref<ID3D11ShaderResourceView> SRV_VB_Src;
		Ref<ID3D11ShaderResourceView> SRV_VB_Tessed;
		Ref<ID3D11ShaderResourceView> SRV_EdgeFactor;

		Ref<ID3D11ShaderResourceView> SRV_VB_Scatter;
		Ref<ID3D11ShaderResourceView> SRV_IB_Scatter;

		Ref<ID3D11UnorderedAccessView> UAV_EdgeFactor;

		Ref<ID3D11UnorderedAccessView> UAV_VB_Scatter;
		Ref<ID3D11UnorderedAccessView> UAV_IB_Scatter;

		Ref<ID3D11UnorderedAccessView> UAV_VB_Tessed;
		Ref<ID3D11UnorderedAccessView> UAV_IB_Tessed;
	}m_Res;

	Scanner m_Scanner;

	uint32_t m_SrcVertexCount = 0U;
	uint32_t m_CachedTessedVertexCount = 0U;
	uint32_t m_CachedTessedIndexCount = 0U;
};
