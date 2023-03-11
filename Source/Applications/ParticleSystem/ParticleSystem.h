#pragma once

#include "D3DGraphic.h"

class ParticleSystem
{
public:
	enum eParticleType
	{
		eFire,
		eRain,
		eTypeCount
	};

	ParticleSystem() = default;
	~ParticleSystem() = default;

	void Init();
	void Draw(const class Camera &cam);

	void Update(float elapseTime, float totalTime);

	inline void SetType(eParticleType type)
	{
		assert(type < eTypeCount);
		m_Type = type;
	}

	inline void EnableWireframe(bool bWireFrame)
	{
		m_bWireframe = bWireFrame;
	}
protected:
private:
	enum eVertexType
	{
		eEmitter,
		eParticle
	};

	struct ParticleVertex
	{
		Vec3 InitialPos = { 0.0f, 0.0f, 0.0f };
		Vec3 InitialVelocity = { 0.0f, 0.0f, 0.0f };

		Vec2 Size = { 0.0f, 0.0f };
		float ElapseTime = 0.0f;
		eVertexType Type = eEmitter;
	};

	struct D3DResource
	{
		Ref<ID3D11InputLayout> InputLayout;

		Ref<ID3D11Buffer> CBuf;

		Ref <ID3D11ShaderResourceView> SRVRandomTex;

		Ref<ID3D11Buffer> VBSrc;
		Ref<ID3D11Buffer> VBDraw[eTypeCount];
		Ref<ID3D11Buffer> VBStreamOut[eTypeCount];

		Ref<ID3D11ShaderResourceView> SRVTextures[eTypeCount];

		Ref<ID3D11VertexShader> VSStreamOut[eTypeCount];
		Ref<ID3D11GeometryShader> GSStreamOut[eTypeCount];

		Ref<ID3D11VertexShader> VSDraw[eTypeCount];
		Ref<ID3D11GeometryShader> GSDraw[eTypeCount];

		Ref<ID3D11PixelShader> PixelShaders[eTypeCount];

		Ref<ID3D11RasterizerState> Wireframe;
		Ref<ID3D11RasterizerState> NullCulling;
		
		Ref<ID3D11BlendState> AlphaToCoverage;
		Ref<ID3D11BlendState> Transparent;
		Ref<ID3D11BlendState> AdditiveBlending;

		Ref<ID3D11SamplerState> SamplerLinear;
		
		Ref<ID3D11DepthStencilState> DisableDepth;
		Ref<ID3D11DepthStencilState> NoDepthWrites;
	}m_Res;

	eParticleType m_Type = eFire;

	bool m_bInited = false;
	bool m_bWireframe = false;
};
