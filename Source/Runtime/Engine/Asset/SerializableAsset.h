#pragma once

#include "Runtime/Engine/Asset/Asset.h"
#include "Runtime/Engine/Engine.h"
#include "Runtime/Core/PlatformMisc.h"
#include "Runtime/Core/Cereal.h"

#define SERIALIZABLE_EXT ".json"
///#define SERIALIZABLE_MEMORY_BLOCK_SIZE 128u

template<class T>
class SerializableAsset : public Asset
{
public:
	using Asset::Asset;
	using ParentClass = SerializableAsset<T>;

	template<class... TArgs>
	static std::shared_ptr<T> Load(const char8_t* Path, TArgs&&... Args)
	{
		auto NewAsset = std::make_shared<T>(Path, std::forward<TArgs>(Args)...);
		NewAsset->Load();

		return NewAsset;
	}

	void Load()
	{
		std::ifstream InFileStream(GetPath());
		if (InFileStream.is_open())
		{
			cereal::JSONInputArchive Ar(InFileStream);
			Ar(
				cereal::make_nvp(typeid(T).name(), *static_cast<T*>(this))
			);
		}
		else
		{
			LOG_TRACE("Create serializable asset: \"{}\".", GetPath());
			Save(true);
		}
	}

	void Save(bool8_t Force = false, const char8_t* CustomPath = nullptr)
	{
		if (IsDirty() || Force)
		{
			std::string SavePath = CustomPath ? CustomPath : GetPath();
			
			if (!std::filesystem::path(SavePath).has_extension())
			{
				SavePath += SERIALIZABLE_EXT;
			}

			auto ParentPath = std::filesystem::path(SavePath).parent_path();
			if (!ParentPath.empty() && !std::filesystem::exists(ParentPath))
			{
				VERIFY(std::filesystem::create_directories(ParentPath));
			}

			std::ofstream OutFileStream(SavePath);
			if (OutFileStream.is_open())
			{
				cereal::JSONOutputArchive Ar(OutFileStream);
				Ar(
					cereal::make_nvp(typeid(T).name(), *static_cast<T*>(this))
				);
			}
			else
			{
				LOG_ERROR("Failed to save serializable asset: \"{}\", {}", SavePath, PlatformMisc::GetErrorMessage());
			}
		}
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Asset)
		);
	}
};

