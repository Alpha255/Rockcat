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

struct AssetRawData
{
	size_t SizeInBytes = 0u;
	std::unique_ptr<byte8_t> Data;
private:
	friend class Asset;

	void Allocate(size_t DataSize)
	{
		assert(DataSize > 0u);
		SizeInBytes = DataSize;
		Data.reset(new byte8_t[DataSize]());
	}
};

struct AssetType
{
	enum class EContentsType
	{
		Text,
		Binary
	};

	std::string_view Name;
	std::string_view Extension;
	EContentsType ContentsType;

	AssetType(const char8_t* InName, const char8_t* InExtension, EContentsType InContentsType = EContentsType::Binary)
		: Name(InName)
		, Extension(InExtension)
		, ContentsType(InContentsType)
	{
	}
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

	using AssetPreLoadCallback = std::function<void(Asset&)>;
	using AssetReadyCallback = std::function<void(Asset&)>;
	using AssetReloadCallback = std::function<void(Asset&)>;
	using AssetLoadFailedCallback = std::function<void(Asset&)>;
	using AssetCanceledCallback = std::function<void(Asset&)>;

	using AssetSavedCallback = std::function<void(Asset&)>;
	using AssetUnloadedCallback = std::function<void(Asset&)>;

	struct Callbacks
	{
		AssetPreLoadCallback PreLoadCallback;
		AssetReadyCallback ReadyCallback;
		AssetReloadCallback ReloadCallback;
		AssetLoadFailedCallback LoadFailedCallback;
		AssetCanceledCallback CanceledCallback;
		AssetSavedCallback SavedCallback;
		AssetUnloadedCallback UnloadedCallback;
	};

	template<class StringType>
	Asset(StringType&& Path)
		: m_Path(std::forward<StringType>(Path))
		, m_LastWriteTime(GetFileLastWriteTime(m_Path))
	{
	}

	EAssetStatus GetStatus() const { return m_Status.load(); }
	bool8_t IsReady() const { return GetStatus() == EAssetStatus::Ready; }
	bool8_t IsLoading() const { return GetStatus() == EAssetStatus::Loading; }
	const AssetRawData& GetRawData() const { return m_RawData; }
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

	void SetCallbacks(std::optional<Callbacks>& InCallbacks)
	{ 
		if (InCallbacks) 
		{ 
			m_Callbacks = std::move(InCallbacks.value()); 
		} 
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_LastWriteTime)
		);
	}

	static std::optional<Callbacks> s_DefaultNullCallbacks;
protected:
	friend class AssetImportTask;

	void SetStatus(EAssetStatus Status) { m_Status.store(Status); }
	void ReadRawData(AssetType::EContentsType ContentsType);

	virtual void OnPreLoad() { if (m_Callbacks.PreLoadCallback) { m_Callbacks.PreLoadCallback(*this); } }
	virtual void OnReady() { if (m_Callbacks.ReadyCallback) { m_Callbacks.ReadyCallback(*this); } }
	virtual void OnLoadFailed() { if (m_Callbacks.LoadFailedCallback) { m_Callbacks.LoadFailedCallback(*this); } }
	virtual void OnCanceledLoad() { if (m_Callbacks.CanceledCallback) { m_Callbacks.CanceledCallback(*this); } }
	virtual void OnSaved() { if (m_Callbacks.SavedCallback) { m_Callbacks.SavedCallback(*this); } }
	virtual void OnUnloaded() { if (m_Callbacks.UnloadedCallback) { m_Callbacks.UnloadedCallback(*this); } }

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
	AssetRawData m_RawData;
	std::atomic<EAssetStatus> m_Status = EAssetStatus::NotLoaded;

	Callbacks m_Callbacks;

	std::filesystem::path m_Path; /// Notice the order of the members
	mutable std::time_t m_LastWriteTime = 0u;
	mutable bool8_t m_Dirty = false;
};

class IAssetImporter
{
public:
	IAssetImporter(std::vector<AssetType>&& ValidAssetTypes)
		: m_ValidAssetTypes(std::move(ValidAssetTypes))
	{
	}

	const AssetType* FindValidAssetType(const std::filesystem::path& Extension) const
	{
		auto It = std::find_if(m_ValidAssetTypes.begin(), m_ValidAssetTypes.end(), [Extension](const AssetType& Type) {
			return _stricmp(Extension.generic_string().c_str(), Type.Extension.data()) == 0;
			});
		return It == m_ValidAssetTypes.cend() ? nullptr : &(*It);
	}

	virtual std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) = 0;
	virtual bool8_t Reimport(Asset& InAsset) = 0;
protected:
private:
	std::vector<AssetType> m_ValidAssetTypes;
};
