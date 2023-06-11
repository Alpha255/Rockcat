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

	template<class... TArgs>
	static std::shared_ptr<T> Load(const char8_t* Path, TArgs&&... Args)
	{
		if (!s_ThisAsset)
		{
			s_ThisAsset = std::make_shared<T>(Path, std::forward<TArgs>(Args)...);
			s_ThisAsset->Load();
			s_ThisAsset->SetStatus(Asset::EAssetStatus::Ready);
		}

		return s_ThisAsset;
	}

	template<class... TArgs>
	static std::shared_ptr<T> Load(const std::string& Path, TArgs&&... Args)
	{
		return Load(Path.c_str(), std::forward<TArgs>(Args)...);
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
		InFileStream.close();
	}

	void Save(bool8_t Force = false, const char8_t* CustomPath = nullptr)
	{
		if (IsDirty() || Force)
		{
			std::string SavePath = CustomPath ? CustomPath : GetPath();
			
			if (!std::filesystem::path(SavePath).has_extension())
			{
				SavePath += GetExtension();
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
			OutFileStream.close();
		}
	}

	virtual const char8_t* GetExtension() const { return Asset::GetPrefabricateAssetExtension(Asset::EPrefabricateAssetType::ConfigAsset); }

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

template <class T> std::shared_ptr<T> SerializableAsset<T>::s_ThisAsset;

