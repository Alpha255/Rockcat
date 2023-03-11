#pragma once

#include "Public/Model.h"

struct Scene
{
public:
	enum eConfig
	{
		eShadowCount = 1U,
		eMirrorCount = 4U,
		eLightCount = 4U
	};

	enum eDrawType
	{
		eMirror,
		eShadow = eMirror + eMirrorCount,
		eScene,
		eTypeCount
	};

	struct LightParam
	{
		Matrix VP;

		Vec4 Position;
		Vec4 Direction;
		Vec4 Color;
		Vec4 Falloff;
	};

	struct ConstantsBufferVS
	{
		Matrix World;
		Matrix WorldInverse;
		Matrix VP;
	};

	struct ConstantsBufferPS
	{
		Vec4 MirrorPlane;
		Vec4 TintColor = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

		Vec4 AmbientColor = Vec4(0.04f * 0.760f, 0.04f * 0.793f, 0.04f * 0.822f, 1.0f);
		LightParam LightParams[eLightCount];

		ConstantsBufferPS();

		Matrix CalcLightMatrix(uint32_t iLight, float fov, float aspect, float nearPlane, float farPlane);
	};

	struct CBufferVS
	{
		ConstantsBufferVS Memory;
		Matrix LightVP[eLightCount];
		RBuffer Buffer;
		std::mutex Mutex;

		void Update(const Matrix &world, const Matrix &vp, RContext &context, bool bUpdateIM = false);
	};

	struct CBufferPS
	{
		ConstantsBufferPS Memory;
		RBuffer Buffer;
		std::mutex Mutex;

		inline void SetMirror(const Vec4 &mirrorPlane, const Vec4 &tintColor)
		{
			std::lock_guard<std::mutex> locker(Mutex);

			Memory.MirrorPlane = mirrorPlane;
			Memory.TintColor = tintColor;
		}

		inline void SetLight(const LightParam &lightParam, uint32_t index)
		{
			std::lock_guard<std::mutex> locker(Mutex);

			Memory.LightParams[index] = lightParam;
		}

		void Update(RContext &context);
	};

	struct StaticParams
	{
		Vec4 TintColor = {};
		Vec4 MirrorPlane = {};
	};

	struct Shadow
	{
		enum eConfig
		{
			eShadowMapSize = 2048U
		};

		RShaderResourceView ShadowMap;
		RDepthStencilView DepthMap;

		StaticParams Params;

		void Initialize();
	};

	struct Mirror
	{
		typedef Geometry::Vertex MirrorRect[4U];

		MirrorRect Vertices[eMirrorCount];
		Matrix World[eMirrorCount];
		StaticParams Params[eMirrorCount];

		RBuffer VertexBuffer;
		RInputLayout Layout;

		void Initialize(RVertexShader &vertexShader);
	};

	void Initialize();
	void Draw(const DXUTCamera &camera, uint32_t width, uint32_t height);
	void DrawByPart(const DXUTCamera &camera, uint32_t width, uint32_t height, eDrawType type, RContext &context);
	void Update(float elapsedTime, float totalTime);

protected:
	void SetMirror(const DXUTCamera &camera, uint32_t index, RContext &context);
	void DrawMirror(const DXUTCamera &camera, uint32_t index, RContext &context);
	void DrawShadow(const DXUTCamera &camera, uint32_t width, uint32_t height, RContext &context);
	void DrawScene(const DXUTCamera &camera, const StaticParams &params, const Matrix &world, const Matrix &vp, bool bShadow, RContext &context);
	void DrawSceneNormally(const DXUTCamera &camera, RContext &context);

public:
	RVertexShader VertexShader;
	RPixelShader PixelShader;

	CBufferVS CBuffer_VS;
	CBufferPS CBuffer_PS;

	StaticParams Params;

	RDepthStencilState NoStencil;
	RDepthStencilState DepthTestStencilOverwrite;
	RDepthStencilState DepthOverwriteStencilTest;
	RDepthStencilState DepthWriteStencilTest;
	RDepthStencilState DepthOverwriteStencilClear;

	Shadow ShadowRes;
	Mirror MirrorRes;

	Geometry::Model SquidRoom;

	static const float s_SceneRadius;
};
