#pragma once

#include "Runtime/Engine/Asset/Asset.h"
#include "Runtime/Engine/Services/SpdLogService.h"
#include "Runtime/Core/PlatformMisc.h"
#include "Runtime/Core/Cereal.h"

///#define SERIALIZABLE_MEMORY_BLOCK_SIZE 128u

template<class T>
class SerializableAsset : public Asset
{
public:
	using Asset::Asset;
	using BaseClass = SerializableAsset<T>;

	template<class Type = T, class... TArgs>
	static std::shared_ptr<Type> Load(TArgs&&... Args)
	{
		static std::shared_ptr<Type> s_Asset;
		if (!s_Asset)
		{
			s_Asset = std::make_shared<Type>(std::forward<TArgs>(Args)...);
			s_Asset->Reload<Type>();
		}

		return s_Asset;
	}

	template<class Type = T>
	void Reload()
	{
		SetStatus(Asset::EAssetStatus::Loading);
		PreLoad();

		std::ifstream InFileStream(GetPath());
		if (InFileStream.is_open())
		{
			cereal::JSONInputArchive Ar(InFileStream);
			Ar(
				cereal::make_nvp(typeid(Type).name(), *static_cast<Type*>(this))
			);
		}
		else
		{
			LOG_TRACE("Create serializable asset: \"{}\".", GetPath().generic_string());
			Save<Type>(true);
		}
		InFileStream.close();

		PostLoad();
		SetStatus(Asset::EAssetStatus::Ready);
	}

	template<class Type = T>
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
					cereal::make_nvp(typeid(Type).name(), *static_cast<Type*>(this))
				);
			}
			else
			{
				LOG_ERROR("Failed to save serializable asset: \"{}\", {}", SavePath.generic_string(), PlatformMisc::GetErrorMessage());
			}
			OutFileStream.close();
		}
	}

	virtual const char8_t* GetExtension() const { return Asset::GetPrefabricateAssetExtension(Asset::EPrefabAssetType::Config); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Asset)
		);
	}
protected:
	virtual void PreLoad() {}
	virtual void PostLoad() {}
};

