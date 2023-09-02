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
	std::shared_ptr<byte8_t> Data;
private:
	friend class Asset;

	void Allocate(size_t DataSize)
	{
		assert(DataSize > 0u);
		SizeInBytes = DataSize;
		Data.reset(new byte8_t[DataSize]());
	}

	void Deallocate() { Data.reset(); SizeInBytes = 0u; }
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

	enum class EPrefabAssetType : uint8_t
	{
		Config,
		Scene,
		ShaderCache,
		Material
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
	virtual bool8_t IsReady() const { return GetStatus() == EAssetStatus::Ready; }
	bool8_t IsLoading() const { return GetStatus() == EAssetStatus::Loading; }

	const std::filesystem::path& GetPath() const { return m_Path; }

	std::time_t GetLastWriteTime() const { return m_LastWriteTime; }

	void ReadRawData(AssetType::EContentsType ContentsType);
	void FreeRawData() { m_RawData.Deallocate(); }
	const AssetRawData& GetRawData() const { return m_RawData; }

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

	static const char8_t* GetPrefabricateAssetExtension(EPrefabAssetType Type)
	{
		switch (Type)
		{
		case EPrefabAssetType::Config: return ".json";
		case EPrefabAssetType::Scene: return ".scene";
		case EPrefabAssetType::ShaderCache: return ".shadercache";
		case EPrefabAssetType::Material: return ".material";
		default: return ".json";
		}
	}

	template<class StringType>
	static std::filesystem::path GetPrefabricateAssetPath(StringType&& AssetName, EPrefabAssetType Type)
	{
		auto Ret = std::filesystem::path();
		switch (Type)
		{
		case EPrefabAssetType::Config:
			break;
		case EPrefabAssetType::Scene:
			Ret += ASSET_PATH_SCENES;
			break;
		case EPrefabAssetType::ShaderCache:
			Ret += ASSET_PATH_SHADERCACHE;
			break;
		case EPrefabAssetType::Material:
			Ret += ASSET_PATH_MATERIALS;
			break;
		default:
			break;
		}
		Ret += AssetName;
		if (Ret.extension().empty())
		{
			Ret += GetPrefabricateAssetExtension(Type);
		}
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

	virtual void OnPreLoad() { if (m_Callbacks.PreLoadCallback) { m_Callbacks.PreLoadCallback(*this); } }
	virtual void OnReady() { if (m_Callbacks.ReadyCallback) { m_Callbacks.ReadyCallback(*this); } }
	virtual void OnLoadFailed() { if (m_Callbacks.LoadFailedCallback) { m_Callbacks.LoadFailedCallback(*this); } }
	virtual void OnLoadCanceled() { if (m_Callbacks.CanceledCallback) { m_Callbacks.CanceledCallback(*this); } }
	virtual void OnSaved() { if (m_Callbacks.SavedCallback) { m_Callbacks.SavedCallback(*this); } }
	virtual void OnUnloaded() { if (m_Callbacks.UnloadedCallback) { m_Callbacks.UnloadedCallback(*this); } }

	template<class StringType>
	void SetPath(StringType&& Path) { m_Path = std::filesystem::path(std::forward<StringType>(Path)); }

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

	virtual void LoadAssetData(std::shared_ptr<Asset>& Asset, AssetType::EContentsType ContentsType) 
	{ 
		Asset->ReadRawData(ContentsType); 
	}

	virtual std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) = 0;
	virtual bool8_t Reimport(Asset& InAsset) = 0;
protected:
private:
	std::vector<AssetType> m_ValidAssetTypes;
};

struct MemoryBlock
{
	MemoryBlock() = default;

	MemoryBlock(size_t DataSize, const byte8_t* const InData)
		: SizeInBytes(DataSize)
		, RawData(new byte8_t[DataSize]())
	{
		VERIFY(memcpy_s(RawData.get(), DataSize, InData, DataSize) == 0);
	}

	size_t SizeInBytes = 0u;
	std::shared_ptr<byte8_t> RawData;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseClass),
			CEREAL_NVP(SizeInBytes)
		);

		if (Archive::is_loading::value)
		{
			std::vector<uint64_t> Buffer;
			Ar(
				cereal::make_nvp("RawData", Buffer)
			);
			RawData.reset(new byte8_t[SizeInBytes]);
			memcpy_s(RawData.get(), SizeInBytes, Buffer.data(), SizeInBytes);
		}
		else
		{
			std::vector<uint64_t> Buffer(SizeInBytes / sizeof(uint64_t) + 1u);
			memcpy_s(Buffer.data(), SizeInBytes, RawData.get(), SizeInBytes);
			Ar(
				cereal::make_nvp("RawData", Buffer)
			);
		}
	}
};
