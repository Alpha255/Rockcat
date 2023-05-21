#pragma once

#include "Runtime/Core/Definitions.h"

class Profiler
{
public:
	enum ETag : uint8_t
	{
		None = -1
	};

	struct TagData
	{
		ETag Enum = ETag::None;
		ETag ParentEnum = ETag::None;
		std::string_view DisplayName;
	};

	class Event
	{
	public:
		Event(ETag Enum, ETag ParentEnum = ETag::None)
		{
			Start();
		}

		virtual ~Event()
		{
			Stop();
		}
	protected:
		virtual void Start() = 0;
		virtual void Stop() = 0;

		void PushTag()
		{
		}

		void PopTag()
		{
		}
	};

	void RegisterTagData(ETag Enum, ETag ParentEnum, const char8_t* Name, const char8_t* ParentName)
	{
		assert(Enum != ETag::None && Name && std::strlen(Name) > 0u);
	}
protected:
private:
	std::vector<std::shared_ptr<TagData>> m_Tags;
};

