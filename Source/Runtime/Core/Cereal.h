#pragma once

#include "Core/Definitions.h"
#include "Core/String.h"

//#define CEREAL_THREAD_SAFE 1

#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>
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
#include <cereal/types/variant.hpp>

template<class Enum>
struct EnumSerializer
{
	Enum& Value;

	EnumSerializer(Enum& InValue)
		: Value(InValue)
	{
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		std::string EnumName;
		if constexpr (Archive::is_saving::value)
		{
			EnumName = std::string(magic_enum::enum_name<Enum>(Value));
			Ar(EnumName);
		}
		else if constexpr (Archive::is_loading::value)
		{
			Ar(EnumName);
			auto OptValue = magic_enum::enum_cast<Enum>(EnumName);
			if (OptValue.has_value())
			{
				Value = OptValue.value();
			}
		}
	}
};

#define CEREAL_BASE(ClassType) ::cereal::make_nvp(typeid(ClassType).name(), ::cereal::virtual_base_class<ClassType>(this))

#define CEREAL_SERIALIZE_ENUM(EnumType, Value)                               \
	if constexpr (Archive::is_saving::value) {                               \
		auto EnumName = std::string(magic_enum::enum_name<EnumType>(Value)); \
		Ar(::cereal::make_nvp(#Value, EnumName));                            \
	}                                                                        \
	else if constexpr (Archive::is_loading::value) {                         \
		std::string EnumName;                                                \
		Ar(::cereal::make_nvp(#Value, EnumName));                            \
		auto OptValue = magic_enum::enum_cast<EnumType>(EnumName);           \
		if (OptValue.has_value()) {                                          \
			Value = OptValue.value();                                        \
		}                                                                    \
	}

#define CEREAL_NVP_ENUM(EnumType, Value) ::cereal::make_nvp(#Value, EnumSerializer<EnumType>(Value))

namespace cereal
{
	template<class Archive>
	void serialize(Archive& Ar, std::filesystem::path& Path)
	{
		std::string PathStr;
		if constexpr (Archive::is_saving::value)
		{
			PathStr = Path.string();
			Ar(PathStr);
		}
		else if constexpr (Archive::is_loading::value)
		{
			Ar(PathStr);
			Path = std::move(std::filesystem::path(PathStr));
		}
	}

	// ::string publicly derives from std::string, so cereal's std::basic_string save/load
	// (cereal/types/string.hpp / json.hpp) already pick it up through derived-to-base deduction.
	// Do NOT add a serialize() overload here: that would give ::string two serializers and
	// trigger cereal's C2338 ("found more than one compatible serialization function").
	//
	// However, deduction alone is not enough for the JSON archives: binding ::string to
	// std::basic_string<...> const& is a derived-to-base *conversion*, which ranks worse than
	// the identity match of cereal's generic prologue(JSONOutputArchive&, T const&). So the
	// generic "object" prologue/epilogue win, calling startNode()/finishNode() around a value
	// that was written as a plain string, which makes rapidjson throw.
	// These exact-type (non-template) overloads win the overload resolution and make the JSON
	// archives treat ::string exactly like std::string.

	inline void prologue(JSONOutputArchive& Ar, ::string const&)
	{
		Ar.writeName();
	}

	inline void epilogue(JSONOutputArchive&, ::string const&)
	{
	}

	inline void prologue(JSONInputArchive&, ::string const&)
	{
	}

	inline void epilogue(JSONInputArchive&, ::string const&)
	{
	}

	inline void save(JSONOutputArchive& Ar, ::string const& Str)
	{
		Ar.saveValue(static_cast<std::string const&>(Str));
	}

	inline void load(JSONInputArchive& Ar, ::string& Str)
	{
		Ar.loadValue(static_cast<std::string&>(Str));
	}
}
