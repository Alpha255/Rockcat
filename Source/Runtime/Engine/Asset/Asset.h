#pragma once

#include "Core/ObjectID.h"

struct DataBlock
{
	size_t Size = 0u;
	size_t Offset = 0u;
	std::shared_ptr<std::byte> Data;

	DataBlock() = default;

	DataBlock(size_t InSize, const void* InData = nullptr, size_t InOffset = 0u)
		: Size(InSize)
		, Offset(InOffset)
		, Data(new std::byte[InSize + InOffset]())
	{
		assert(Offset < Size);

		if (InData)
		{
			VERIFY(memcpy_s(Data.get() + Offset, InSize, InData, InSize) == 0);
		}
	}
	
	inline bool IsValid() const { return Size && Data; }

	template<class Archive>
	void load(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Size),
			CEREAL_NVP(Offset)
		);

		Data.reset(new std::byte[Size + Offset]());

		Ar.loadBinaryValue(Data.get() + Offset, Size, "Data");
	}

	template<class Archive>
	void save(Archive& Ar) const
	{
		Ar(
			CEREAL_NVP(Size),
			CEREAL_NVP(Offset)
		);

		Ar.saveBinaryValue(Data.get() + Offset, Size, "Data");
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
	std::filesystem::path Extension;
	EContentsType ContentsType;

	AssetType(const char* InName, const char* InExtension, EContentsType InContentsType = EContentsType::Binary)
		: Name(InName)
		, Extension(InExtension)
		, ContentsType(InContentsType)
	{
	}
};

class Asset
{
public:
	enum class EStatus : uint8_t
	{
		NotLoaded,
		Loading,
		Ready,
		Error
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

	Asset(const std::filesystem::path& Path)
		: m_Path(Path)
		, m_LastWriteTime(GetLastWriteTime(m_Path))
	{
	}

	Asset(std::filesystem::path&& Path)
		: m_Path(std::move(Path))
		, m_LastWriteTime(GetLastWriteTime(m_Path))
	{
	}

	virtual ~Asset() = default;

	EStatus GetStatus() const { return m_Status.load(std::memory_order_relaxed); }
	virtual bool IsReady() const { return GetStatus() == EStatus::Ready; }

	const std::filesystem::path& GetPath() const { return m_Path; }
	std::filesystem::path GetName() const { return m_Path.filename(); }
	std::filesystem::path GetStem() const { return m_Path.stem(); }
	std::filesystem::path GetExtension() const { return m_Path.extension(); }
	std::time_t GetLastWriteTime() const { return m_LastWriteTime; }

	void FreeRawData() { m_RawData.reset(); }
	const DataBlock& GetRawData(AssetType::EContentsType ContentsType) const;

	bool IsDirty() const
	{
		std::time_t LastWriteTime = m_LastWriteTime;
		m_LastWriteTime = GetLastWriteTime(m_Path);
		return m_LastWriteTime != LastWriteTime;
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

	static std::time_t GetLastWriteTime(const std::filesystem::path& Path)
	{
		if (std::filesystem::exists(Path))
		{
			return std::chrono::duration_cast<std::filesystem::file_time_type::duration>(
				std::filesystem::last_write_time(Path).time_since_epoch()).count();
		}
		return 0;
	}

	static size_t GetSize(const std::filesystem::path& Path)
	{
		if (std::filesystem::exists(Path))
		{
			return std::filesystem::file_size(Path);
		}
		return 0u;
	}
protected:
	friend struct AssetImportTask;

	void SetStatus(EStatus Status) { m_Status.store(Status, std::memory_order_release); }

	virtual void OnPreLoad() { if (m_Callbacks.PreLoadCallback) { m_Callbacks.PreLoadCallback(*this); } }
	virtual void OnReady() { if (m_Callbacks.ReadyCallback) { m_Callbacks.ReadyCallback(*this); } }
	virtual void OnLoadFailed() { if (m_Callbacks.LoadFailedCallback) { m_Callbacks.LoadFailedCallback(*this); } }
	virtual void OnLoadCanceled() { if (m_Callbacks.CanceledCallback) { m_Callbacks.CanceledCallback(*this); } }
	virtual void OnSaved() { if (m_Callbacks.SavedCallback) { m_Callbacks.SavedCallback(*this); } }
	virtual void OnUnloaded() { if (m_Callbacks.UnloadedCallback) { m_Callbacks.UnloadedCallback(*this); } }

	mutable std::unique_ptr<DataBlock> m_RawData;
	std::atomic<EStatus> m_Status = EStatus::NotLoaded;

	Callbacks m_Callbacks;

	std::filesystem::path m_Path; /// Notice the order of the members
	mutable std::time_t m_LastWriteTime = 0u;
};

class IAssetImporter
{
public:
	IAssetImporter(std::vector<AssetType>&& ValidAssetTypes)
		: m_ValidAssetTypes(std::move(ValidAssetTypes))
	{
	}

	const AssetType* FindValidAssetType(const std::filesystem::path& Path) const
	{
		auto It = std::find_if(m_ValidAssetTypes.begin(), m_ValidAssetTypes.end(), [&Path](const AssetType& Type) {
			return Path.extension() == Type.Extension;
		});
		return It == m_ValidAssetTypes.cend() ? nullptr : &(*It);
	}

	virtual void LoadAssetData(std::shared_ptr<Asset>& Asset, AssetType::EContentsType ContentsType)
	{
		Asset->GetRawData(ContentsType);
	}

	virtual std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) = 0;
	virtual bool Reimport(Asset& InAsset, const AssetType& InAssetType) = 0;
protected:
private:
	std::vector<AssetType> m_ValidAssetTypes;
};
