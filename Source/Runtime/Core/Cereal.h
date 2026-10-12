#pragma once

#include "Core/Definitions.h"
#include "Core/String.h"

#include <algorithm>
#include <cstring>

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
	//
	// save()/load() are constrained to std::string-derived types on purpose: as plain
	// ::string-overloads they are also viable for char[N]/char const* (the array decays to a
	// pointer and ::string has a converting constructor), which gives char[N] a second output
	// serializer next to cereal's own C array support and trips cereal's C2338.

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

	template<class T, std::enable_if_t<std::is_base_of_v<std::string, T>, int> = 0>
	inline void save(JSONOutputArchive& Ar, T const& Str)
	{
		Ar.saveValue(static_cast<std::string const&>(Str));
	}

	template<class T, std::enable_if_t<std::is_base_of_v<std::string, T>, int> = 0>
	inline void load(JSONInputArchive& Ar, T& Str)
	{
		Ar.loadValue(static_cast<std::string&>(Str));
	}

	// C style character arrays (e.g. ImFontConfig::Name) round-trip as a single string instead of
	// the element wise array cereal would write by default (cereal/types/common.hpp). Being more
	// specialized than cereal's generic C array support, this wins the overload resolution and
	// leaves char[N] with exactly one serializer.
	template<class Archive, size_t N>
	void serialize(Archive& Ar, char (&Str)[N])
	{
		if constexpr (Archive::is_saving::value)
		{
			Ar.saveValue(static_cast<char const*>(Str));
		}
		else if constexpr (Archive::is_loading::value)
		{
			std::string Value;
			Ar.loadValue(Value);

			const size_t Length = std::min(Value.size(), N - 1);
			std::memcpy(Str, Value.data(), Length);
			Str[Length] = '\0';
		}
	}

	// char[N] is written as a plain value, so it must not open/close a node the way cereal's
	// generic (non-arithmetic) JSON prologue/epilogue would.
	template<size_t N>
	void prologue(JSONOutputArchive& Ar, char const (&)[N])
	{
		Ar.writeName();
	}

	template<size_t N>
	void epilogue(JSONOutputArchive&, char const (&)[N])
	{
	}

	template<size_t N>
	void prologue(JSONInputArchive&, char const (&)[N])
	{
	}

	template<size_t N>
	void epilogue(JSONInputArchive&, char const (&)[N])
	{
	}
}