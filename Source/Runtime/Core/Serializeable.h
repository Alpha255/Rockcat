#pragma once

#include "Runtime/Core/File.h"
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/base_class.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/deque.hpp>
#include <cereal/types/forward_list.hpp>
#include <cereal/types/list.hpp>

#define SERIALIZEABLE_EXT ".json"

template<class T>
class Serializeable : public File
{
public:
	using File::File;

	template<class... TArgs>
	static std::shared_ptr<T> Load(const char8_t* Path, TArgs&&... Args)
	{
		auto Temp = std::make_shared<T>(Path, std::forward<TArgs>(Args)...);
		Temp->Load();

		return Temp;
	}

	void Load()
	{
		std::ifstream Fs(Path());
		if (Fs.is_open())
		{
			cereal::JSONInputArchive Ar(Fs);
			Ar(
				cereal::make_nvp(typeid(T).name(), *static_cast<T*>(this))
			);
			///ar.serializeDeferments();
		}
		else
		{
			LOG_WARNING("Failed to find file: \"{}\", create a new one.", Path());
			Save(true);
		}

		OnLoadCompletion();
	}

	void Save(bool8_t Force = false, const char8_t* OtherPath = nullptr)
	{
		if (Force || IsDirty())
		{
			std::string TargetPath = OtherPath ? OtherPath : Path();
			auto Ext = File::Extension(TargetPath, true);
			if (Ext.length() == 0)
			{
				TargetPath += SERIALIZEABLE_EXT;
			}

			auto Directory = File::Directory(TargetPath);
			if (!File::Exists(Directory.c_str()))
			{
				File::CreateDirectories(Directory.c_str());
			}

			std::ofstream Fs(TargetPath);

			if (Fs.is_open())
			{
				LastWriteTime();
				cereal::JSONOutputArchive Ar(Fs);
				Ar(
					cereal::make_nvp(typeid(T).name(), *static_cast<T*>(this))
				);
				///ar.serializeDeferments();
			}
			else
			{
				LOG_ERROR("Failed to save file: \"{}\", System error: \"{}\"", TargetPath, Platform::GetErrorMessage(~0u));
			}
		}
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(File)
		);
	}

	bool8_t IsDirty() const
	{
		return m_Dirty;
	}

	void SetDirty(bool8_t Dirty = true)
	{
		m_Dirty = Dirty;
	}
protected:
	virtual void OnLoadCompletion() {}
private:
	bool8_t m_Dirty = false;
};

