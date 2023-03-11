#pragma once

#include "Core/Gear.h"

#define ASSET_PATH_SHADERS      "Shaders\\"
#define ASSET_PATH_SHADER_CACHE "Shaders\\Cache\\"
#define ASSET_PATH_TEXTURES     "Textures\\"
#define ASSET_PATH_IMAGES       "Assets\\Images\\"
#define ASSET_PATH_MATERIALS    "Materials\\"
#define ASSET_PATH_MODELS       "Models\\"
#define ASSET_PATH_AUDIOS       "Audios\\"
#define ASSET_PATH_SCENES       "Scenes\\"
#define ASSET_PATH_SETTINGS     "Settings\\"

struct IAsset
{
	enum ECategory
	{
		Model,
		Image,
		Shader,
		Unknown,
	};

	enum class ELoadingStatus
	{
		None,
		Loading,
		Loaded
	};

	std::atomic<ELoadingStatus> Status;

	static ECategory Category() { return ECategory::Unknown; }

	static std::string CatPath(const char8_t* BasePath, const char8_t* SubPath)
	{
		assert(BasePath);
		return String::Format("%s%s", BasePath, SubPath ? SubPath : "");
	}

	static std::string Path(ECategory Category, const char8_t* DstPath)
	{
		assert(DstPath);

		std::string AssetPath(DstPath);
		if (File::Exists(AssetPath.c_str()))
		{
			return AssetPath;
		}

		switch (Category)
		{
		case IAsset::ECategory::Model:
			AssetPath = CatPath(ASSET_PATH_MODELS, DstPath);
			break;
		case IAsset::ECategory::Image:
			AssetPath = CatPath(ASSET_PATH_TEXTURES, DstPath);
			break;
		case IAsset::ECategory::Shader:
			AssetPath = CatPath(ASSET_PATH_SHADERS, DstPath);
			break;
		}

		return AssetPath;
	}

	ELoadingStatus LoadingStatus() const
	{
		return Status.load();
	}

	void SetLoadingStatus(ELoadingStatus status)
	{
		Status.store(status);
	}
};

using AssetID = Handle<IAsset, uint32_t>;

template<class T, IAsset::ECategory TCategory>
struct Asset : public IAsset
{
	using CompletionCallback = std::function<void(std::shared_ptr<Asset>)>;
	using CompletionCallbackList = ThreadSafeContainer<std::vector<CompletionCallback>>;

	Asset() = default;

	Asset(const std::string& AssetPath)
		: Path(AssetPath)
	{
	}

	static ECategory Category() { return TCategory; }
	std::shared_ptr<T> Object;
	std::string Path;
	CompletionCallbackList CompletionCallbacks;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Path)
		);
	}
};
