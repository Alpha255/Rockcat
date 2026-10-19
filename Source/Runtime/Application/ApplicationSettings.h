#pragma once

#include "Asset/JsonAsset.h"

enum class EWindowMode
{
	Windowed,
	BorderlessFullscreen,
	ExclusiveFullscreen
};

enum class ERenderBackend
{
	Software,
	Vulkan,
	D3D12,
	D3D11,
	OpenGL,
	Num
};

struct ApplicationSettings : public JsonAsset<ApplicationSettings>
{
	uint32_t WindowWidth = 1280u;
	uint32_t WindowHeight = 720u;
	EWindowMode WindowMode = EWindowMode::Windowed;

	ERenderBackend RenderBackend = ERenderBackend::Vulkan;
};