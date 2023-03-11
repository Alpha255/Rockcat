#include "Scanner.h"

extern D3DGraphic* g_Renderer;

struct ConstantsBuf
{
	uint32_t Params[4] = { 0 };
};

void Scanner::Init(uint32_t vertexCount)
{
	assert(g_Renderer);

	static char *s_ShaderName = "Scanner.hlsl";
	g_Renderer->CreateComputeShader(m_Res.ScanInBucket, s_ShaderName, "Func_ScanInBucket");
	g_Renderer->CreateComputeShader(m_Res.ScanBucketResult, s_ShaderName, "Func_ScanBucketResult");
	g_Renderer->CreateComputeShader(m_Res.ScanAddBucketResult, s_ShaderName, "Func_ScanAddBucketResult");

	g_Renderer->CreateConstantBuffer(m_Res.CB_Scan, sizeof(ConstantsBuf), D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);

	Ref<ID3D11Buffer> auxBuf;
	g_Renderer->CreateUnorderedAccessBuffer(auxBuf, sizeof(uint32_t) * 2U * 128U, D3D11_USAGE_DEFAULT,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(uint32_t) * 2);

	g_Renderer->CreateShaderResourceView(m_Res.SRV_Aux, auxBuf.Ptr(), DXGI_FORMAT_UNKNOWN, 
		D3D11_SRV_DIMENSION_BUFFER, 0U, 128U);

	g_Renderer->CreateUnorderedAccessView(m_Res.UAV_Aux, auxBuf.Ptr(), DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER, 0U, 128U);

	g_Renderer->CreateUnorderedAccessBuffer(m_Res.UOB_ScanBuf0, sizeof(uint32_t) * 2 * vertexCount / 3, D3D11_USAGE_DEFAULT,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(uint32_t) * 2);
	g_Renderer->CreateShaderResourceView(m_Res.SRV_Scan0, m_Res.UOB_ScanBuf0.Ptr(), DXGI_FORMAT_UNKNOWN,
		D3D11_SRV_DIMENSION_BUFFER, 0U, vertexCount / 3);
	g_Renderer->CreateUnorderedAccessView(m_Res.UAV_Scan0, m_Res.UOB_ScanBuf0.Ptr(), DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER, 0U, vertexCount / 3);

	g_Renderer->CreateUnorderedAccessBuffer(m_Res.UOB_ScanBuf1, sizeof(uint32_t) * 2 * vertexCount / 3, D3D11_USAGE_DEFAULT,
		D3D11_RESOURCE_MISC_BUFFER_STRUCTURED, sizeof(uint32_t) * 2);
	g_Renderer->CreateShaderResourceView(m_Res.SRV_Scan1, m_Res.UOB_ScanBuf1.Ptr(), DXGI_FORMAT_UNKNOWN,
		D3D11_SRV_DIMENSION_BUFFER, 0U, vertexCount / 3);
	g_Renderer->CreateUnorderedAccessView(m_Res.UAV_Scan1, m_Res.UOB_ScanBuf1.Ptr(), DXGI_FORMAT_UNKNOWN,
		D3D11_UAV_DIMENSION_BUFFER, 0U, vertexCount / 3);
}

void Scanner::Scan(uint32_t numToScan)
{
	static Ref<ID3D11UnorderedAccessView> s_NullUAV;
	static Ref<ID3D11ShaderResourceView> s_NullSRV;

	/// First pass, scan in each bucket
	{
		g_Renderer->SetComputeShader(m_Res.ScanInBucket);
		g_Renderer->SetShaderResource(m_Res.SRV_Scan0, 0U, D3DGraphic::eComputeShader);
		g_Renderer->SetUnorderedAccessView(m_Res.UAV_Scan1);
		g_Renderer->ExeCommandlist((uint32_t)ceilf(numToScan / 128.0f), 1U, 1U);
		g_Renderer->SetUnorderedAccessView(s_NullUAV);
	}

	/// Second pass, record and scan the sum of each bucket
	{
		g_Renderer->SetComputeShader(m_Res.ScanBucketResult);
		g_Renderer->SetShaderResource(m_Res.SRV_Scan1, 0U, D3DGraphic::eComputeShader);
		g_Renderer->SetUnorderedAccessView(m_Res.UAV_Aux);
		g_Renderer->ExeCommandlist(1U, 1U, 1U);
		g_Renderer->SetUnorderedAccessView(s_NullUAV);
	}

	/// Last pass, add the buckets scanned result to each bucket to get the final result
	{
		g_Renderer->SetComputeShader(m_Res.ScanAddBucketResult);
		g_Renderer->SetShaderResource(m_Res.SRV_Scan1, 0U, D3DGraphic::eComputeShader);
		g_Renderer->SetShaderResource(m_Res.SRV_Aux, 1U, D3DGraphic::eComputeShader);
		g_Renderer->SetUnorderedAccessView(m_Res.UAV_Scan0);
		g_Renderer->ExeCommandlist((uint32_t)ceilf(numToScan / 128.0f), 1U, 1U);
	}

	/// Restore state
	g_Renderer->SetUnorderedAccessView(s_NullUAV);
	g_Renderer->SetShaderResource(s_NullSRV, 0U, D3DGraphic::eComputeShader);
	g_Renderer->SetShaderResource(s_NullSRV, 1U, D3DGraphic::eComputeShader);
}
