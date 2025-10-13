#pragma once

#include "Asset/File.h"

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

	inline void Reset()
	{
		Size = Offset = 0u;
		Data.reset();
	}

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
	enum class EContentsFormat
	{
		PlainText,
		Binary
	};

	std::string_view Description;
	std::string_view Extension;
	EContentsFormat ContentsFormat = EContentsFormat::Binary;
};

class Asset : public File
{
public:
	enum class EStatus : uint8_t
	{
		NotLoaded,
		Loading,
		Ready,
		LoadFailed
	};

	using File::File;

	inline EStatus GetStatus(std::memory_order MemoryOrder = std::memory_order_relaxed) const { return m_Status.load(MemoryOrder); }
	inline bool IsReady(std::memory_order MemoryOrder = std::memory_order_relaxed) const { return GetStatus(MemoryOrder) == EStatus::Ready; }
	inline bool IsOnLoading(std::memory_order MemoryOrder = std::memory_order_relaxed) const { return GetStatus(MemoryOrder) == EStatus::Loading; }

	std::shared_ptr<DataBlock> LoadData(AssetType::EContentsFormat ContentsFormat) const;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(File)
		);
	}
protected:
	friend class AssetLoadTask;

	inline void SetStatus(EStatus Status, std::memory_order MemoryOrder = std::memory_order_release) { m_Status.store(Status, MemoryOrder); }

	virtual void OnPreLoad() { SetStatus(EStatus::Loading); }
	virtual void OnReady() {}
	virtual void OnCancel() { SetStatus(EStatus::NotLoaded); }
	virtual void OnPostLoad() { SetStatus(EStatus::Ready); }
	virtual void OnUnload() { SetStatus(EStatus::NotLoaded); }
	virtual void OnReload() {}
	virtual void OnLoadFailed(const std::string& ErrorMessage);

	std::atomic<EStatus> m_Status = EStatus::NotLoaded;
};

class AssetLoader
{
public:
	AssetLoader(std::vector<AssetType>&& SupportedAssetTypes)
		: m_SupportedTypes(std::move(SupportedAssetTypes))
	{
	}

	inline bool IsSupportedAssetType(const std::filesystem::path& Path) const
	{
		return TryFindAssetTypeByPath(Path) != nullptr;
	}

	const AssetType* TryFindAssetTypeByPath(const std::filesystem::path& Path) const
	{
		auto It = std::find_if(m_SupportedTypes.cbegin(), m_SupportedTypes.cend(), [&Path](const AssetType& Type) {
			return Path.extension() == Type.Extension;
			});
		return It == m_SupportedTypes.cend() ? nullptr : &(*It);
	}

	virtual bool Load(Asset& InAsset, const AssetType& InType, std::string& ErrorMessage) = 0;
protected:
	friend class AssetLoadTask;

	virtual std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& Path) = 0;
private:
	std::vector<AssetType> m_SupportedTypes;
};
