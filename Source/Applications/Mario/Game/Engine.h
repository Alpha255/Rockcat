#pragma once

#include "Common.h"
#include "Objects.h"
#include "Map.h"
#include "Image.h"
#include "D3DShader.h"
#include "D3DBuffer.h"
#include "D3DState.h"

#include <list>

class Engine
{
public:
	Engine(const Engine &) = delete;
	void operator= (const Engine &) = delete;

	static Engine &Instance()
	{
		if (!s_Instance)
		{
			s_Instance = std::unique_ptr<Engine, std::function<void(Engine *)>>
				(new Engine(), [](Engine *pEngine) { SafeDelete(pEngine) });
			assert(s_Instance);
		}

		return *s_Instance;
	}

	void Init(HWND hWnd, uint32_t width, uint32_t height);

	void Update(float elapseTime, float totalTime);
	void RenderScene();
	void DrawMap();

	void HandleInput(uint32_t msg, WPARAM wParam, LPARAM lParam);

	void Resize(uint32_t width, uint32_t height);

	void AddObject(Object2D::eType type, uint32_t left, uint32_t top);

	inline const Image *GetImage(uint32_t index) const
	{
		assert(index < m_Textures.size());
		return &m_Textures[index];
	}

	inline void SetMap(Map *pMap)
	{
		m_CurrentMap = pMap == nullptr ? m_CurrentMap : pMap;
	}
protected:
	struct Vertex
	{
		Vec3 Position;
		Vec2 UV;
	};

	Engine() = default;
	~Engine() = default;

	void LoadTextures();
	void LoadMaps();
	void InitD3DResource();

	void DrawObject(const Object2D &object);
	void DrawStaticObjects();
	void DrawDynamicObjects();

	void UpdateVertexBuffer(const Object2D &object);
private:
	static std::unique_ptr<Engine, std::function<void(Engine *)>> s_Instance;

	Map *m_CurrentMap = nullptr;

	std::array<Image, Object2D::eTypeCount> m_Textures;
	std::array<Map, Map::eMapCount> m_Maps;
	std::array<Vertex, 4U> m_Vertices;
	std::array<uint32_t, 6U> m_Indices = { 0U, 1U, 2U, 0U, 2U, 3U };

	std::list<std::shared_ptr<Object2D>> m_Objects;

	Vec4 m_BackColor{ 107.0f / 255.0f, 136.0f / 255.0f, 255.0f / 255.0f, 1.0f };
	Vec4 m_AlphaColor{ 255.0f / 255.0f, 60.0f / 255.0f, 174.0f / 255.0f, 1.0f };

	D3DInputLayout m_VertexLayout;
	D3DBuffer m_VertexBuffer;
	D3DBuffer m_IndexBuffer;
	D3DVertexShader m_VertexShader;
	D3DPixelShader m_PixelShader;
	D3DPixelShader m_PixelShaderDark;
	D3DBlendState m_AlphaBlend;
	D3DViewport m_Viewport;
};
