#pragma once

#include "Runtime/Engine/Asset/Asset.h"
#include "Runtime/Engine/Engine.h"
#include "Runtime/Core/PlatformMisc.h"
#include "Runtime/Core/Cereal.h"

///#define SERIALIZABLE_MEMORY_BLOCK_SIZE 128u

template<class T>
class SerializableAsset : public Asset
{
public:
	using Asset::Asset;
	using ParentClass = SerializableAsset<T>;

	template<class StringType, class... TArgs>
	static std::shared_ptr<T> Load(StringType&& Path, TArgs&&... Args)
	{
		if (!s_ThisAsset)
		{
			s_ThisAsset = std::make_shared<T>(std::forward<StringType>(Path), std::forward<TArgs>(Args)...);
			s_ThisAsset->Reload();
		}

		return s_ThisAsset;
	}

	void Reload()
	{
		SetStatus(Asset::EAssetStatus::Loading);
		PreLoad();

		std::ifstream InFileStream(GetPath());
		if (InFileStream.is_open())
		{
			cereal::JSONInputArchive Ar(InFileStream);
			Ar(
				cereal::make_nvp(typeid(*this).name(), *static_cast<T*>(this))
			);
		}
		else
		{
			LOG_TRACE("Create serializable asset: \"{}\".", GetPath().generic_string());
			Save(true);
		}
		InFileStream.close();

		PostLoad();
		SetStatus(Asset::EAssetStatus::Ready);
	}

	void Save(bool8_t Force = false, const std::filesystem::path& CustomPath = std::filesystem::path())
	{
		if (IsDirty() || Force)
		{
			std::filesystem::path SavePath = CustomPath.empty() ? GetPath() : CustomPath;
			
			if (!SavePath.has_extension())
			{
				SavePath += GetExtension();
			}

			auto ParentPath = SavePath.parent_path();
			if (!ParentPath.empty() && !std::filesystem::exists(ParentPath))
			{
				VERIFY(std::filesystem::create_directories(ParentPath));
			}

			std::ofstream OutFileStream(SavePath);
			if (OutFileStream.is_open())
			{
				cereal::JSONOutputArchive Ar(OutFileStream);
				Ar(
					cereal::make_nvp(typeid(*this).name(), *static_cast<T*>(this))
				);
			}
			else
			{
				LOG_ERROR("Failed to save serializable asset: \"{}\", {}", SavePath.generic_string(), PlatformMisc::GetErrorMessage());
			}
			OutFileStream.close();
		}
	}

	virtual const char8_t* GetExtension() const { return Asset::GetPrefabricateAssetExtension(Asset::EPrefabricateAssetType::ConfigAsset); }

	virtual void PreLoad() {}
	virtual void PostLoad() {}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Asset)
		);
	}
private:
	static std::shared_ptr<T> s_ThisAsset;
};

template<class T> std::shared_ptr<T> SerializableAsset<T>::s_ThisAsset;

