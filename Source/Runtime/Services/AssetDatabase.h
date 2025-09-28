#pragma once

#include "Core/Module.h"
#include "Core/StringUtils.h"
#include "Asset/Asset.h"

class AssetDatabase : public IService<AssetDatabase>
{
public:
	void Initialize() override final;
	void Finalize() override final;

	template<class T>
	inline std::shared_ptr<T> LoadAsync(const std::filesystem::path& Path)
	{
		return LoadAssetImpl(GetUnifyAssetPath(Path), true);
	}

	template<class T>
	inline std::shared_ptr<T> Load(const std::filesystem::path& Path)
	{
		return LoadAssetImpl(GetUnifyAssetPath(Path), true);
	}
private:
	inline static std::filesystem::path GetUnifyAssetPath(const std::filesystem::path& Path, bool Lowercase = false)
	{
		auto UnifyPath = std::filesystem::path(std::move(std::filesystem::path(Path).make_preferred()));
		return Lowercase ? std::filesystem::path(StringUtils::Lowercase(UnifyPath.string())) : UnifyPath;
	}

	void CreateAssetLoaders();

	std::shared_ptr<Asset> LoadAssetImpl(const std::filesystem::path& Path, bool Async);

	std::unordered_map<std::filesystem::path, std::shared_ptr<class AssetLoadTask>> m_AssetLoadTasks;
	std::vector<std::unique_ptr<AssetLoader>> m_AssetLoaders;
};

