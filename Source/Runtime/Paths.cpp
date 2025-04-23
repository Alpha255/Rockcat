#include "Paths.h"
#include "System.h"

const std::filesystem::path& Paths::RootPath()
{
	static std::filesystem::path Path;
	if (Path.empty())
	{
		Path = System::GetCurrentModuleDirectory().parent_path();
	}

	return Path;
}

const std::filesystem::path& Paths::AssetPath()
{
	static std::filesystem::path Path;
	if (Path.empty())
	{
		Path = RootPath() / "Assets";
	}

	return Path;
}

const std::filesystem::path& Paths::ShaderPath()
{
	static std::filesystem::path Path;
	if (Path.empty())
	{
		Path = AssetPath() / "Shaders";
	}

	return Path;
}

const std::filesystem::path& Paths::ShaderBinaryCachePath()
{
	static std::filesystem::path Path;
	if (Path.empty())
	{
		Path = ShaderPath() / "ShaderBinaryCache";
	}

	return Path;
}

const std::filesystem::path& Paths::MaterialPath()
{
	static std::filesystem::path Path;
	if (Path.empty())
	{
		Path = AssetPath() / "Materials";
	}
	
	return Path;
}

const std::filesystem::path& Paths::AudioPath()
{
	static std::filesystem::path Path;
	if (Path.empty())
	{
		Path = AssetPath() / "Audio";
	}

	return Path;
}

const std::filesystem::path& Paths::ScenePath()
{
	static std::filesystem::path Path;
	if (Path.empty())
	{
		Path = AssetPath() / "Scenes";
	}

	return Path;
}

const std::filesystem::path& Paths::GltfSampleModelPath()
{
	static std::filesystem::path Path;
	if (Path.empty())
	{
		Path = AssetPath() / "glTF-Sample-Assets" / "Models";
	}

	return Path;
}

const std::filesystem::path& Paths::ConfigPath()
{
	static std::filesystem::path Path;
	if (Path.empty())
	{
		Path = AssetPath() / "Configs";
	}

	return Path;
}

const std::filesystem::path& Paths::FontPath()
{
	static std::filesystem::path Path;
	if (Path.empty())
	{
		Path = AssetPath() / "Fonts";
	}

	return Path;
}

const std::filesystem::path& Paths::EditorThemePath()
{
	static std::filesystem::path Path;
	if (Path.empty())
	{
		Path = ConfigPath() / "EditorThemes";
	}

	return Path;
}
