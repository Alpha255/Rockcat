#include "Tessellator.h"
#include "Camera.h"

extern D3DGraphic* g_Renderer;

struct CB_EdgeFactor
{
	Matrix WVP;

	Vec2 TessEdgeLenScale;
	uint32_t TriCount;
	uint32_t PartitioningMode;
};

void Tessellator::CreateLookupTable()
{	
	uint32_t halfFactor = s_MaxFactor / 2U;
	
	int32_t finalPointPosTable[s_MaxFactor / 2 + 1] = { 0 };
	finalPointPosTable[1] = halfFactor;

	/// Init final_Point_Position_Table
	for (uint32_t i = 2U; i < halfFactor + 1; ++i)
	{
		int32_t level = 0;
		for (;;)
		{
			if (0 == (((i - 2) - ((1UL << level) - 1)) & ((1UL << (level + 1)) - 1)))
			{
				break;
			}
			++level;
		}

		finalPointPosTable[i] = ((halfFactor >> 1) + ((i - 2) - ((1UL << level) - 1))) >> (level + 1);
	}

	/// Init inside_Point_Index_Table / outside_Point_Index_Table
	for (uint32_t i = 0U; i <= halfFactor; ++i)
	{
		for (uint32_t j = 0U; j <= halfFactor; ++j)
		{
			m_InsidePointIndex[i][j][0] = (0 == j ? 0 : (finalPointPosTable[j] < (int32_t)i));

			m_OutsidePointIndex[i][j][0] = (finalPointPosTable[j] < (int32_t)i);
		}

		m_InsidePointIndex[i][halfFactor + 1][0] = 0;
		m_OutsidePointIndex[i][halfFactor + 1][0] = 0;

		for (uint32_t k = 0; k <= halfFactor + 1; ++k)
		{
			m_InsidePointIndex[i][k][1] = (0 == k ? 0 : (m_InsidePointIndex[i][k - 1][0] + m_InsidePointIndex[i][k - 1][1]));

			m_OutsidePointIndex[i][k][1] = (0 == k ? 0 : (m_OutsidePointIndex[i][k - 1][0] + m_OutsidePointIndex[i][k - 1][1]));

			if (m_InsidePointIndex[i][k][0])
			{
				m_InsidePointIndex[i][m_InsidePointIndex[i][k][1]][2] = k;
			}

			if (m_OutsidePointIndex[i][k][0])
			{
				m_OutsidePointIndex[i][m_OutsidePointIndex[i][k][1]][2] = k;
			}
		}

		for (int32_t m = halfFactor; m >= 0; --m)
		{
			if (m_InsidePointIndex[i][m][0])
			{
				m_InsidePointIndex[i][m_InsidePointIndex[i][halfFactor + 1][1] - m_InsidePointIndex[i][m + 1][1]][3] = m;
			}

			if (m_OutsidePointIndex[i][m][0])
			{
				m_OutsidePointIndex[i][m_OutsidePointIndex[i][halfFactor + 1][1] - m_OutsidePointIndex[i][m + 1][1]][3] = m;
			}
		}
	}
}

void Tessellator::CreateResource(uint32_t vertexCount, ID3D11Buffer *pVertexBuf)
{
	assert(g_Renderer && pVertexBuf);

	g_Renderer->CreateComputeShader(m_Res.CS_EdgeFactor, "Tessellator_EdgeFactor.hlsl", "Func_EdgeFactor");

	g_Renderer->CreateComputeShader(m_Res.CS_ScatterVertexTriID_IndexID, "Tessellator_ScatterID.hlsl", "Func_ScatterVertexTriID_IndexID");
	g_Renderer->CreateComputeShader(m_Res.CS_ScatterIndexTriID_IndexID, "Tessellator_ScatterID.hlsl", "Func_ScatterIndexTriID_IndexID");

	g_Renderer->CreateComputeShader(m_Res.CS_NumVI, "Tessellator_NumVI.hlsl", "Func_NumVI");

	g_Renderer->CreateComputeShader(m_Res.CS_TessVertex, "Tessellator_TessVertex.hlsl", "Func_TessVertex");

	g_Renderer->CreateComputeShader(m_Res.CS_TessIndex, "Tessellator_TessIndex.hlsl", "Func_TessIndex");

	static const size_t s_LUTSize = (sizeof(m_InsidePointIndex) + sizeof(m_OutsidePointIndex)) / sizeof(int32_t);
	struct ConstantsBuf
	{
		int32_t LUT[s_LUTSize] = { 0 };
		
		uint32_t PartitionMode;
		uint32_t Dummy0 = 0U;
		uint32_t Dummy1 = 0U;
		uint32_t Dummy2 = 0U;
	};

	ConstantsBuf cbuf;
	memcpy(cbuf.LUT, m_InsidePointIndex, sizeof(m_InsidePointIndex));
	memcpy(cbuf.LUT + sizeof(m_InsidePointIndex) / sizeof(int32_t), m_OutsidePointIndex, sizeof(m_OutsidePointIndex));
	cbuf.PartitionMode = (uint32_t)m_ptMode;

	g_Renderer->CreateConstantBuffer(m_Res.CB_LookupTable, sizeof(ConstantsBuf), D3D11_USAGE_IMMUTABLE, &cbuf);

	g_Renderer->CreateConstantBuffer(m_Res.CB_EdgeFactor, sizeof(CB_EdgeFactor), D3D11_USAGE_DEFAULT, nullptr);

	g_Renderer->CreateConstantBuffer(m_Res.CB_Tess, sizeof(uint32_t) * 4, D3D11_USAGE_DEFAULT, nullptr);

	g_Renderer->CreateReadOnlyBuffer(m_Res.CB_ReadBackBuf, sizeof(uint32_t) * 2, nullptr, D3D11_CPU_ACCESS_READ);

	g_Renderer->CreateShaderResourceView(m_Res.SRV_VB_Src, pVertexBuf, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_SRV_DIMENSION_BUFFER, 0U, vertexCount);

	Ref<ID3D11Buffer> edgeFactor;
	g_Renderer->CreateUnorderedAccessBuffer(edgeFactor, sizeof(float) * vertexCount / 3U * 4U, 
		D3D11_USAGE_DEFAULT, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(float) * 4);
	g_Renderer->CreateShaderResourceView(m_Res.SRV_EdgeFactor, edgeFactor.Ptr(),
		DXGI_FORMAT_UNKNOWN, D3D11_SRV_DIMENSION_BUFFER, 0U, vertexCount / 3);
	g_Renderer->CreateUnorderedAccessView(m_Res.UAV_EdgeFactor, edgeFactor.Ptr(),
		DXGI_FORMAT_UNKNOWN, D3D11_UAV_DIMENSION_BUFFER, 0U, vertexCount / 3);
}

void Tessellator::ExeComputeShader(
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
	uint32_t patchZ)
{
	g_Renderer->SetComputeShader(rTargetCS);
	for (uint32_t i = 0U; i < srvCount; ++i)
	{
		Ref<ID3D11ShaderResourceView> srv(ppSRVs[i]);
		g_Renderer->SetShaderResource(srv, i, D3DGraphic::eComputeShader);
	}
	g_Renderer->SetUnorderedAccessView(rUAV);

	if (rCBUpdate.Valid())
	{
		g_Renderer->UpdateBuffer(rCBUpdate.Ptr(), 0U, nullptr, pBufData, cbufSize, cbufSize);
	}

	if (rCBConstant.Valid() && rCBUpdate.Valid())
	{
		g_Renderer->SetConstantBuffer(rCBConstant, 0U, D3DGraphic::eComputeShader);
		g_Renderer->SetConstantBuffer(rCBUpdate, 1U, D3DGraphic::eComputeShader);
	}

	if (rCBConstant.Valid() && !rCBUpdate.Valid())
	{
		g_Renderer->SetConstantBuffer(rCBConstant, 0U, D3DGraphic::eComputeShader);
	}

	if (!rCBConstant.Valid() && rCBUpdate.Valid())
	{
		g_Renderer->SetConstantBuffer(rCBUpdate, 0U, D3DGraphic::eComputeShader);
	}

	g_Renderer->ExeCommandlist(patchX, patchY, patchZ);

	static Ref<ID3D11UnorderedAccessView> s_NullUAV;
	static Ref<ID3D11ShaderResourceView> s_NullSRV;
	static Ref<ID3D11Buffer> s_NullBuffer;

	g_Renderer->SetUnorderedAccessView(s_NullUAV);
	for (uint32_t i = 0U; i < srvCount; ++i)
	{
		g_Renderer->SetShaderResource(s_NullSRV, i, D3DGraphic::eComputeShader);
	}
	g_Renderer->SetConstantBuffer(s_NullBuffer, 1U, D3DGraphic::eComputeShader);
}

bool Tessellator::DoTessellationByEdge(const Camera &cam)
{
	uint32_t tessedVertexCount = 0U, tessedIndexCount = 0U;

	static Ref<ID3D11Buffer> s_NullBuffer;

	/// Update per-edge tessellation factors
	{
		CB_EdgeFactor cbuf;
		cbuf.WVP = cam.GetWorldMatrix() * cam.GetViewMatrix() * cam.GetProjMatrix();
		cbuf.PartitioningMode = m_ptMode;
		cbuf.TessEdgeLenScale = m_EdgeLenScale;
		cbuf.TriCount = m_SrcVertexCount / 3U;

		ID3D11ShaderResourceView *ppSRV[1] = { m_Res.SRV_VB_Src.Ptr() };
		ExeComputeShader(
			m_Res.CS_EdgeFactor,
			1U,
			ppSRV,
			s_NullBuffer,
			m_Res.CB_EdgeFactor,
			sizeof(CB_EdgeFactor),
			&cbuf,
			m_Res.UAV_EdgeFactor,
			(uint32_t)ceilf(m_SrcVertexCount / 3.0f / 128.0f),
			1U,
			1U);
	}

	/// How many vertices/indices are needed for the tessellated mesh?
	{
		uint32_t cbuf[4] = { m_SrcVertexCount / 3U, 0U, 0U, 0U };
		ID3D11ShaderResourceView *ppSRV[1] = { m_Res.SRV_EdgeFactor.Ptr() };
		ExeComputeShader(
			m_Res.CS_NumVI,
			1U,
			ppSRV,
			m_Res.CB_LookupTable,
			m_Res.CB_Tess,
			sizeof(uint32_t) * 4U,
			cbuf,
			m_Scanner.GetScanUAV0(),
			(uint32_t)ceilf(m_SrcVertexCount / 3.0f / 128.0f),
			1U,
			1U);
		m_Scanner.Scan(m_SrcVertexCount / 3U);

		/// Read back the number of vertices/indices for tessellation output
		uint32_t copyBuf[2] = { 0U };
		::RECT srcRect =
		{
			(long)(sizeof(uint32_t) * 2L * m_SrcVertexCount / 3L - sizeof(uint32_t) * 2L),
			0L,
			(long)(sizeof(uint32_t) * 2L * m_SrcVertexCount / 3L),
			1L
		};
		g_Renderer->CopyBuffer(m_Res.CB_ReadBackBuf.Ptr(), m_Scanner.GetScanBuf0().Ptr(), copyBuf, sizeof(uint32_t) * 2U, srcRect);
		tessedVertexCount = copyBuf[0];
		tessedIndexCount = copyBuf[1];
	}

	if (0U == tessedVertexCount || 0U == tessedIndexCount)
	{
		/// Tessellate failed
		return false;
	}

	/// Generate buffers for scattering TriID and IndexID for both vertex data and index data,
	/// also generate buffers for output tessellated vertex data and index data
	static Ref<ID3D11Buffer> NullBuf;
	static Ref<ID3D11ShaderResourceView> NullSRV;
	static Ref<ID3D11UnorderedAccessView> NullUAV;

	{
		if (!m_Res.VB_Scatter.Valid() || m_CachedTessedVertexCount < tessedVertexCount)
		{
			m_Res.VB_Scatter = NullBuf;
			m_Res.SRV_VB_Scatter = NullSRV;
			m_Res.UAV_VB_Scatter = NullUAV;

			m_Res.VB_Tessed = NullBuf;
			m_Res.SRV_VB_Tessed = NullSRV;
			m_Res.UAV_VB_Tessed = NullUAV;

			g_Renderer->CreateUnorderedAccessBuffer(m_Res.VB_Scatter, sizeof(uint32_t) * 2U * tessedVertexCount,
				D3D11_USAGE_DEFAULT, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(uint32_t) * 2U);
			g_Renderer->CreateShaderResourceView(m_Res.SRV_VB_Scatter, m_Res.VB_Scatter.Ptr(), DXGI_FORMAT_UNKNOWN,
				D3D11_SRV_DIMENSION_BUFFER, 0U, tessedVertexCount);
			g_Renderer->CreateUnorderedAccessView(m_Res.UAV_VB_Scatter, m_Res.VB_Scatter.Ptr(), DXGI_FORMAT_UNKNOWN,
				D3D11_UAV_DIMENSION_BUFFER, 0U, tessedVertexCount);

			/// Create the output tessellated vertices buffer
			g_Renderer->CreateUnorderedAccessBuffer(m_Res.VB_Tessed, sizeof(float) * 3U * tessedVertexCount,
				D3D11_USAGE_DEFAULT, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(float) * 3U);
			g_Renderer->CreateShaderResourceView(m_Res.SRV_VB_Tessed, m_Res.VB_Tessed.Ptr(), DXGI_FORMAT_UNKNOWN,
				D3D11_SRV_DIMENSION_BUFFER, 0U, tessedVertexCount);
			g_Renderer->CreateUnorderedAccessView(m_Res.UAV_VB_Tessed, m_Res.VB_Tessed.Ptr(), DXGI_FORMAT_UNKNOWN,
				D3D11_UAV_DIMENSION_BUFFER, 0U, tessedVertexCount);

			m_CachedTessedVertexCount = tessedVertexCount;
		}
	}

	{
		if (!m_Res.IB_Tessed.Valid() || m_CachedTessedIndexCount < tessedIndexCount)
		{
			m_Res.IB_Scatter = NullBuf;
			m_Res.SRV_IB_Scatter = NullSRV;
			m_Res.UAV_IB_Scatter = NullUAV;

			m_Res.IB_Tessed = NullBuf;
			m_Res.UAV_IB_Tessed = NullUAV;

			g_Renderer->CreateUnorderedAccessBuffer(m_Res.IB_Scatter, sizeof(uint32_t) * 2U * tessedIndexCount,
				D3D11_USAGE_DEFAULT, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(uint32_t) * 2U);
			g_Renderer->CreateShaderResourceView(m_Res.SRV_IB_Scatter, m_Res.IB_Scatter.Ptr(),
				DXGI_FORMAT_UNKNOWN, D3D11_SRV_DIMENSION_BUFFER, 0U, tessedIndexCount);
			g_Renderer->CreateUnorderedAccessView(m_Res.UAV_IB_Scatter, m_Res.IB_Scatter.Ptr(),
				DXGI_FORMAT_UNKNOWN, D3D11_UAV_DIMENSION_BUFFER, 0U, tessedIndexCount);

			/// Generate the output tessellated indices buffer
			g_Renderer->CreateUnorderedAccessBuffer(m_Res.IB_Tessed, sizeof(uint32_t) * tessedIndexCount,
				D3D11_USAGE_DEFAULT, D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS, sizeof(uint32_t) * 2, D3D11_BIND_INDEX_BUFFER);
			g_Renderer->CreateUnorderedAccessView(m_Res.UAV_IB_Tessed, m_Res.IB_Tessed.Ptr(),
				DXGI_FORMAT_R32_TYPELESS, D3D11_UAV_DIMENSION_BUFFER, 0U, tessedIndexCount, D3D11_BUFFER_UAV_FLAG_RAW);

			m_CachedTessedIndexCount = tessedIndexCount;
		}
	}

	/// Scatter TriID, IndexID
	{
		uint32_t cbuf[4] = { m_SrcVertexCount / 3U, 0U, 0U, 0U };
		ID3D11ShaderResourceView *ppSRV[1] = { m_Scanner.GetScanSRV0().Ptr() };
		ExeComputeShader(
			m_Res.CS_ScatterVertexTriID_IndexID,
			1U,
			ppSRV,
			s_NullBuffer,
			m_Res.CB_Tess,
			sizeof(uint32_t) * 4U,
			cbuf,
			m_Res.UAV_VB_Scatter,
			(uint32_t)(ceilf(m_SrcVertexCount / 3U / 128.0f)),
			1U,
			1U);

		ExeComputeShader(
			m_Res.CS_ScatterIndexTriID_IndexID,
			1U,
			ppSRV,
			s_NullBuffer,
			m_Res.CB_Tess,
			sizeof(uint32_t) * 4U,
			cbuf,
			m_Res.UAV_IB_Scatter,
			(uint32_t)(ceilf(m_SrcVertexCount / 3U / 128.0f)),
			1U,
			1U);
	}

	/// Tessellate vertex
	{
		uint32_t cbuf[4] = { tessedVertexCount, 0U, 0U, 0U };
		ID3D11ShaderResourceView *ppSRV[2] = { m_Res.SRV_VB_Scatter.Ptr(), m_Res.SRV_EdgeFactor.Ptr() };
		ExeComputeShader(
			m_Res.CS_TessVertex,
			2U,
			ppSRV,
			m_Res.CB_LookupTable,
			m_Res.CB_Tess,
			sizeof(uint32_t) * 4U,
			cbuf,
			m_Res.UAV_VB_Tessed,
			(uint32_t)(ceilf(tessedVertexCount / 128.0f)),
			1U,
			1U);
	}

	/// Tessellate indices
	{
		uint32_t cbuf[4] = { tessedIndexCount, 0U, 0U, 0U };
		ID3D11ShaderResourceView *ppSRV[3] = { m_Res.SRV_IB_Scatter.Ptr(), m_Res.SRV_EdgeFactor.Ptr(), m_Scanner.GetScanSRV0().Ptr() };
		ExeComputeShader(
			m_Res.CS_TessIndex,
			3U,
			ppSRV,
			m_Res.CB_LookupTable,
			m_Res.CB_Tess,
			sizeof(uint32_t) * 4U,
			cbuf,
			m_Res.UAV_IB_Tessed,
			(uint32_t)(ceilf(tessedIndexCount / 128.0f)),
			1U,
			1U);
	}

	return tessedVertexCount && tessedIndexCount;
}
