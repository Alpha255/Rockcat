#pragma once

#include "Core/Definitions.h"

class Paths
{
public:
	static const std::filesystem::path& RootPath();
	static const std::filesystem::path& AssetPath();
	static const std::filesystem::path& ShaderPath();
	static const std::filesystem::path& ShaderBinaryCachePath();
	static const std::filesystem::path& MaterialPath();
	static const std::filesystem::path& AudioPath();
	static const std::filesystem::path& ScenePath();
	static const std::filesystem::path& GltfSampleModelPath();
	static const std::filesystem::path& ConfigPath();
	static const std::filesystem::path& FontPath();
	static const std::filesystem::path& EditorThemePath();
};
