#pragma once

#include "Engine/Asset/Asset.h"
#include "Engine/Services/SpdLogService.h"
#include "Core/System.h"
#include "Core/Cereal.h"

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
		auto NewAsset = std::make_shared<Type>(std::forward<TArgs>(Args)...);
		NewAsset->Reload<Type>();

		return NewAsset;
	}

	template<class Type = T>
	void Reload()
	{
		SetStatus(Asset::EStatus::Loading);
		PreLoad();

		std::ifstream FileStream(GetPath());
		if (FileStream.is_open())
		{
			cereal::JSONInputArchive Ar(FileStream);
			Ar(
				cereal::make_nvp(typeid(Type).name(), *static_cast<Type*>(this))
			);
		}
		else
		{
			LOG_TRACE("Create serializable asset: \"{}\".", GetPath().generic_string());
			Save<Type>(true);
		}
		FileStream.close();

		PostLoad();
		SetStatus(Asset::EStatus::Ready);
	}

	template<class Type = T>
	void Save(bool Force = false, const std::filesystem::path& CustomPath = std::filesystem::path())
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

			std::ofstream FileStream(SavePath);
			if (FileStream.is_open())
			{
				cereal::JSONOutputArchive Ar(FileStream);
				Ar(
					cereal::make_nvp(typeid(Type).name(), *static_cast<Type*>(this))
				);
			}
			else
			{
				LOG_ERROR("Failed to save serializable asset: \"{}\", {}", SavePath.generic_string(), System::GetErrorMessage());
			}
			FileStream.close();
		}
	}

	static const char* GetExtension() { return ".json"; }

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

