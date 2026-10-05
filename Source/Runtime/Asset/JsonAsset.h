#pragma once

#include "Core/Cereal.h"
#include "Asset/Asset.h"

template<class T>
class JsonAsset : public Asset
{
public:
	using Asset::Asset;
	using BaseClass = JsonAsset<T>;

	template<class Type = T, class... Args>
	static std::shared_ptr<Type> Load(Args&&... InArgs)
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
			std::filesystem::path SavePath = SaveTo.empty() ? GetPath() : SaveTo;
			
			if (!SavePath.has_extension())
			{
				SavePath += GetExtension();
			}

			auto ParentPath = SavePath.parent_path();
			if (!ParentPath.empty() && !std::filesystem::exists(ParentPath))
			{
				VERIFY(std::filesystem::create_directories(ParentPath));
			}

			std::chrono::system_clock::time_point SerializeTimepoint = std::chrono::system_clock::now();
			SetLastWriteTime(SerializeTimepoint);

			std::ofstream FileStream(SavePath);
			if (FileStream.is_open())
			{
				cereal::JSONOutputArchive Ar(FileStream);
				Ar(
					cereal::make_nvp(typeid(Type).name(), *static_cast<Type*>(this))
				);
			}

			FileStream.close();

			File::SetLastWriteTime(SavePath, SerializeTimepoint);
		}
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Asset)
		);
	}

protected:
	virtual void OnPreLoad() { SetStatus(EStatus::Loading); }

	virtual void OnPostLoad() { SetStatus(EStatus::Ready); }
private:
	static const char* GetExtension() { return ".json"; }
};

