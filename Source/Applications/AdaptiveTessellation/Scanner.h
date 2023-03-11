#pragma once

#include "D3DGraphic.h"

class Scanner
{
public:
	Scanner() = default;
	~Scanner() = default;

	void Init(uint32_t vertexCount);
	void Scan(uint32_t numToScan);

	inline const Ref<ID3D11UnorderedAccessView> &GetScanUAV0() const
	{
		assert(m_Res.UAV_Scan0.Valid());

		return m_Res.UAV_Scan0;
	}

	inline const Ref<ID3D11Buffer> &GetScanBuf0() const 
	{
		assert(m_Res.UOB_ScanBuf0.Valid());

		return m_Res.UOB_ScanBuf0;
	}

	inline const Ref<ID3D11ShaderResourceView> &GetScanSRV0() const
	{
		assert(m_Res.SRV_Scan0.Valid());

		return m_Res.SRV_Scan0;
	}
protected:
private:
	struct D3DResource
	{
		Ref<ID3D11ComputeShader> ScanInBucket;
		Ref<ID3D11ComputeShader> ScanBucketResult;
		Ref<ID3D11ComputeShader> ScanAddBucketResult;

		Ref<ID3D11Buffer> CB_Scan;
		
		Ref<ID3D11Buffer> UOB_ScanBuf0;
		Ref<ID3D11Buffer> UOB_ScanBuf1;

		Ref<ID3D11ShaderResourceView> SRV_Aux;
		Ref<ID3D11ShaderResourceView> SRV_Scan0;
		Ref<ID3D11ShaderResourceView> SRV_Scan1;

		Ref<ID3D11UnorderedAccessView> UAV_Aux;
		Ref<ID3D11UnorderedAccessView> UAV_Scan0;
		Ref<ID3D11UnorderedAccessView> UAV_Scan1;
	}m_Res;
};
