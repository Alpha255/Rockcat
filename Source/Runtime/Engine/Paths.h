#pragma once

#include "Core/Definitions.h"

class Paths
{
public:
	static const std::filesystem::path& RootPath() { return s_Root; }
	static const std::filesystem::path& AssetPath();
	static const std::filesystem::path& ShaderPath();
	static const std::filesystem::path& ShaderBinaryCachePath();
	static const std::filesystem::path& MaterialPath();
	static const std::filesystem::path& AudioPath();
	static const std::filesystem::path& ScenePath();
	static const std::filesystem::path& GltfSampleModelPath();
	static const std::filesystem::path& ConfigPath();
protected:
	friend class Engine;

	static void SetRoot(const std::filesystem::path& Root) { s_Root = Root; }
private:
	static std::filesystem::path s_Root;
};
