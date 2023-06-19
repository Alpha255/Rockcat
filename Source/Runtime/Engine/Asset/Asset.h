#pragma once

#include "Runtime/Core/ObjectID.h"

DECLARE_OBJECT_ID(Asset, uint32_t)

#define ASSET_PATH_SHADERS      "Shaders\\"
#define ASSET_PATH_SHADERCACHE  "Shaders\\ShaderCache\\"
#define ASSET_PATH_IMAGES       "Images\\"
#define ASSET_PATH_MATERIALS    "Materials\\"
#define ASSET_PATH_MODELS       "glTF-Sample-Models\\2.0\\"
#define ASSET_PATH_AUDIOS       "Audios\\"
#define ASSET_PATH_SCENES       "Scenes\\"

struct AssetData
{
	size_t SizeInBytes = 0u;
	std::shared_ptr<byte8_t> RawData;
};

class Asset
{
public:
	enum class EAssetStatus : uint8_t
	{
		NotLoaded,
		Loading,
		Ready,
		Error
	};

	enum class EPrefabricateAssetType : uint8_t
	{
		ConfigAsset,
		SceneAsset,
		ShaderCacheAsset,
		MaterialAsset
	};

	using AssetReadyCallback = std::function<void()>;
	using AssetReloadedCallback = std::function<void()>;
	using AssetErrorCallback = std::function<void()>;
	using AssetCanceledCallback = std::function<void()>;

	using AssetSavedCallback = std::function<void()>;
	using AssetUnloadedCallback = std::function<void()>;

	template<class StringType>
	Asset(StringType&& Path)
		: m_Path(std::forward<StringType>(Path))
		, m_LastWriteTime(GetFileLastWriteTime(m_Path))
	{
	}

	EAssetStatus GetStatus() const { return m_Status.load(); }
	bool8_t IsReady() const { return GetStatus() == EAssetStatus::Ready; }
	bool8_t IsLoading() const { return GetStatus() == EAssetStatus::Loading; }
	const AssetData& GetData() const { return m_Data; }
	const std::filesystem::path& GetPath() const { return m_Path; }
	std::time_t GetLastWriteTime() const { return m_LastWriteTime; }

	bool8_t IsDirty() const
	{
		if (!m_Dirty)
		{
			std::time_t LastWriteTime = m_LastWriteTime;
			m_LastWriteTime = GetFileLastWriteTime(m_Path);
			m_Dirty = m_LastWriteTime != LastWriteTime;
		}
		return m_Dirty;
	}

	static const char8_t* GetPrefabricateAssetExtension(EPrefabricateAssetType Type)
	{
		switch (Type)
		{
		case EPrefabricateAssetType::ConfigAsset: return ".json";
		case EPrefabricateAssetType::SceneAsset: return ".scene";
		case EPrefabricateAssetType::ShaderCacheAsset: return ".shadercache";
		case EPrefabricateAssetType::MaterialAsset: return ".material";
		default: return ".json";
		}
	}

	template<class StringType>
	static std::filesystem::path GetPrefabricateAssetPath(StringType&& AssetName, EPrefabricateAssetType Type)
	{
		auto Ret = std::filesystem::path();
		switch (Type)
		{
		case EPrefabricateAssetType::ConfigAsset:
			break;
		case EPrefabricateAssetType::SceneAsset:
			Ret += ASSET_PATH_SCENES;
			break;
		case EPrefabricateAssetType::ShaderCacheAsset: 
			Ret += ASSET_PATH_SHADERCACHE;
			break;
		case EPrefabricateAssetType::MaterialAsset: 
			Ret += ASSET_PATH_MATERIALS;
			break;
		default:
			break;
		}
		Ret += AssetName;
		return Ret;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_LastWriteTime)
		);
	}
protected:
	friend class AssetImportTask;

	void SetStatus(EAssetStatus Status) { m_Status.store(Status); }

	static std::time_t GetFileLastWriteTime(const std::filesystem::path& Path)
	{
		if (std::filesystem::exists(Path))
		{
			return std::chrono::duration_cast<std::filesystem::file_time_type::duration>(
				std::filesystem::last_write_time(Path).time_since_epoch()).count();
		}
		return 0;
	}

	static size_t GetFileSize(const std::filesystem::path& Path)
	{
		if (std::filesystem::exists(Path))
		{
			return std::filesystem::file_size(Path);
		}
		return 0u;
	}
private:
	AssetData m_Data;
	std::atomic<EAssetStatus> m_Status = EAssetStatus::NotLoaded;

	std::filesystem::path m_Path; /// Notice the order of the members
	mutable std::time_t m_LastWriteTime = 0u;
	mutable bool8_t m_Dirty = false;
};

class IAssetImporter
{
public:
	IAssetImporter(std::initializer_list<const char8_t*> Extensions)
		: m_ValidExtensions(Extensions)
	{
	}

	bool8_t IsValidAssetExtension(const char8_t* Extension) const
	{
		return std::find_if(m_ValidExtensions.begin(), m_ValidExtensions.end(), [Extension](const char8_t* Ext) {
			return _stricmp(Extension, Ext) == 0;
			}) != m_ValidExtensions.end();
	}

	virtual std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) = 0;
	virtual bool8_t Reimport(Asset& InAsset) = 0;
protected:
private:
	std::vector<const char8_t*> m_ValidExtensions;
};
