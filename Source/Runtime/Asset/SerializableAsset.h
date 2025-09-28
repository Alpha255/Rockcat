#pragma once

#include "Core/Cereal.h"
#include "Asset/Asset.h"
#include "Services/SpdLogService.h"

template<class T>
class Serializable : public Asset
{
public:
	using Asset::Asset;
	using BaseClass = Serializable<T>;

	template<class Type = T, class... Args>
	static std::shared_ptr<Type> Load(Args&&... InArgs)
	{
		static std::shared_ptr<T> s_Asset;

		if (!s_Asset)
		{
			s_Asset = std::make_shared<Type>(std::forward<Args>(InArgs)...);
			s_Asset->Reload<Type>();
		}

		return s_Asset;
	}

	template<class Type = T, class... Args>
	static std::shared_ptr<Type> LoadInstance(Args&&... InArgs)
	{
		auto NewAsset = std::make_shared<Type>(std::forward<Args>(InArgs)...);
		NewAsset->Reload<Type>();

		return NewAsset;
	}

	template<class Type = T>
	void Reload()
	{
		OnPreLoad();

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
			LOG_TRACE("Create serializable asset: \"{}\".", GetPath().string());
			Save<Type>(true);
		}

		FileStream.close();

		OnPostLoad();
	}

	template<class Type = T>
	void Save(bool Force = false, const std::filesystem::path& SaveTo = std::filesystem::path())
	{
		if (IsDirty() || Force)
		{
			const std::filesystem::path& SavePath = SaveTo.empty() ? GetPath() : SaveTo;
			
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
				LOG_ERROR("Failed to save serializable asset: \"{}\", {}", SavePath.string());
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
};

