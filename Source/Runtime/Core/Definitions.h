#pragma once

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <array>
#include <stack>
#include <forward_list>
#include <map>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <shared_mutex>
#include <fstream>
#include <set>
#include <unordered_set>
#include <random>
#include <condition_variable>
#include <bitset>
#include <tuple>
#include <optional>
#include <string_view>
#include <filesystem>
#include <variant>
#include <span>
#include <magic_enum/magic_enum.hpp>

#if defined(_WIN32)
	#if !defined(VK_USE_PLATFORM_WIN32_KHR)
		#define VK_USE_PLATFORM_WIN32_KHR 1
	#endif
	#if !defined(PLATFORM_WIN32)
		#define PLATFORM_WIN32 1
	#endif

	#define ENABLE_MULTI_RENDERER 1
#elif defined(__ANDRIOD__)
	#if !defined(VK_USE_PLATFORM_ANDROID_KHR)
		#define VK_USE_PLATFORM_ANDROID_KHR 1
	#endif
	#if !defined(PLATFORM_ANDROID)
		#define PLATFORM_ANDROID 1
	#endif

	#define ENABLE_MULTI_RENDERER 0
#endif

#define NOMINMAX 1

#define NAMESPACE_START(Name) namespace Name {
#define NAMESPACE_END(Name) }

#if defined(DYNAMIC_LIBRARY)
	#define EXPORT_API __declspec(dllexport)
#else
	#define EXPORT_API __declspec(dllimport)
#endif

#define VERIFY(Condition) \
{                         \
	if (!(Condition))     \
	{                     \
		assert(0);        \
	}                     \
}

#if defined(PLATFORM_WIN32)
#define VERIFY_WITH_SYSTEM_MESSAGE(Condition)                                      \
{                                                                                  \
	if (!(Condition))                                                              \
	{                                                                              \
		LOG_ERROR("Failed to invoke platform API, {}", System::GetErrorMessage()); \
		assert(0);                                                                 \
	}                                                                              \
}

#define DLL_EXTENSION ".dll"
#else
	#error Unknown platform!
#endif

#define RENDERER_POSTFIX "Renderer"

#define SPDLOG_LEVEL_NAMES                                              \
    {                                                                   \
        "Trace", "Debug", "Info", "Warning", "Error", "Critical", "Off" \
    }

#define DECLARE_SMART_PTR(ClassName) class ClassName;        \
	using ClassName##SharedPtr = std::shared_ptr<ClassName>; \
	using ClassName##UniquePtr = std::unique_ptr<ClassName>; \
	using ClassName##WeakPtr = std::weak_ptr<ClassName>;

#define DECLARE_RHI_RESOURCE_PTR(ClassName) class ClassName; \
	using ClassName##Ptr = std::shared_ptr<ClassName>;

#define DESCRIPTION(Description)

#define TO_STRING(X) #X
#define CAT(A, B) CAT_INNER(A, B)
#define CAT_INNER(A, B) A##B
#define FILE_LINE CAT(__FILE__, __LINE__)

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

#define CEREAL_SERIALIZE_ENUM(EnumType, Value) \
	if constexpr (Archive::is_saving::value) { \
		auto EnumName = std::string(magic_enum::enum_name<EnumType>(Value)); \
		Ar(::cereal::make_nvp(#Value, EnumName)); \
	} \
	else if constexpr (Archive::is_loading::value) { \
		std::string EnumName; \
		Ar(::cereal::make_nvp(#Value, EnumName)); \
		auto OptValue = magic_enum::enum_cast<EnumType>(EnumName); \
		if (OptValue.has_value()) { \
			Value = OptValue.value(); \
		} \
	}

#define CEREAL_NVP_ENUM(EnumType, Value) ::cereal::make_nvp(#Value, EnumSerializer<EnumType>(Value))

#define ENUM_FLAG_OPERATORS(Enum) \
	inline constexpr Enum    operator|(Enum Left, Enum Right) { return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(Left) | static_cast<std::underlying_type_t<Enum>>(Right)); } \
	inline constexpr Enum    operator&(Enum Left, Enum Right) { return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(Left) & static_cast<std::underlying_type_t<Enum>>(Right)); } \
	inline constexpr Enum    operator^(Enum Left, Enum Right) { return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(Left) ^ static_cast<std::underlying_type_t<Enum>>(Right)); } \
	inline constexpr Enum    operator~(Enum Value) { return static_cast<Enum>(~(static_cast<std::underlying_type_t<Enum>>(Value))); } \
	inline constexpr bool operator!(Enum Value) { return !(static_cast<std::underlying_type_t<Enum>>(Value)); } \
	inline constexpr bool operator!=(Enum Left, Enum Right) { return static_cast<int64_t>(Left) != static_cast<int64_t>(Right); } \
	inline constexpr bool operator==(Enum Left, Enum Right) { return static_cast<int64_t>(Left) == static_cast<int64_t>(Right); }

template<class Enum>
constexpr bool EnumHasAnyFlags(Enum Flags, Enum Contains)
{
	static_assert(std::is_enum_v<Enum>, "Must be enum!");
	return (static_cast<std::underlying_type_t<Enum>>(Flags) & static_cast<std::underlying_type_t<Enum>>(Contains)) != static_cast<std::underlying_type_t<Enum>>(0);
}

template<class Enum1, class Enum2>
constexpr bool EnumHasAnyFlags(Enum1 Flags, Enum2 Contains)
{
	static_assert(std::is_enum_v<Enum1> || std::is_enum_v<Enum2>, "Either Enum1 or Enum2 be enum!");
	return (static_cast<int32_t>(Flags) & static_cast<int32_t>(Contains)) != 0;
}

template<class Enum>
constexpr bool EnumHasAllFlags(Enum Flags, Enum Contains)
{
	static_assert(std::is_enum_v<Enum>, "Must be enum!");
	return (static_cast<std::underlying_type_t<Enum>>(Flags) & static_cast<std::underlying_type_t<Enum>>(Contains)) == static_cast<std::underlying_type_t<Enum>>(Contains);
}

template<class Enum1, class Enum2>
constexpr bool EnumHasAllFlags(Enum1 Flags, Enum2 Contains)
{
	static_assert(std::is_enum_v<Enum1> || std::is_enum_v<Enum2>, "Either Enum1 or Enum2 be enum!");
	return (static_cast<int32_t>(Flags) & static_cast<int32_t>(Contains)) == static_cast<int32_t>(Contains);
}

template<class T>
inline void SafeDelete(T& Object)
{
	if (Object)
	{
		delete Object;
		Object = nullptr;
	}
}

template<class T>
inline void SafeDeleteArray(T& Object)
{
	if (Object)
	{
		delete[] Object;
		Object = nullptr;
	}
}

template<class T>
inline void SafeRelease(T& Object)
{
	if (Object)
	{
		Object->Release();
	}
}

template <class Left, class Right>
inline bool IsEqual(const Left& A, const Right& B)
{
	assert(sizeof(Left) == sizeof(Right));
	return std::memcmp(&A, &B, sizeof(Right)) == 0;
}

/// https://www.boost.org/doc/libs/1_35_0/doc/html/boost/hash_combine_id241013.html
template<class T>
inline void HashCombine(size_t& Seed, const T& Value)
{
	Seed ^= std::hash<T>()(Value) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
}

template<class FirstArg, class... RestArgs>
inline void HashCombine(size_t& Seed, const FirstArg& TheFirstArg, const RestArgs&... TheRestArgs)
{
	HashCombine(Seed, TheFirstArg);
	HashCombine(Seed, TheRestArgs...);
}

template<class... Args>
inline size_t ComputeHash(const Args&... ArgList)
{
	size_t Seed = 0u;
	HashCombine(Seed, ArgList...);
	return Seed;
}

inline constexpr uint64_t FnvHashInner(uint64_t Hash, uint8_t Char)
{
	return (Hash + 0x100000001b3ull) ^ Char;
}

template<size_t Index>
inline constexpr uint64_t FnvHash(uint64_t Hash, const char* Str)
{
	return FnvHash<Index - 1u>(FnvHashInner(Hash, uint8_t(Str[Index])), Str);
}

template<>
inline constexpr uint64_t FnvHash<size_t(-1)>(uint64_t Hash, const char*)
{
	return Hash;
}

template<size_t Length>
inline constexpr uint64_t FnvHash(const char(&Str)[Length])
{
	return FnvHash<Length - 1u>(0xcbf29ce484222325ull, Str);
}

template <class T>
inline constexpr bool IsPowerOfTwo(T Value)
{
	return Value > 0 && (Value & (Value - 1)) == 0;
}

template <class T>
inline constexpr T Align(T Value, T Alignment)
{
	return (Value + (Alignment - 1)) & ~(Alignment - 1);
}

template <class T>
inline constexpr T AlignDown(T Value, T Alignment)
{
	return (Value) & ~(Alignment - 1);
}

template<class Function, class... Args>
inline auto Invoke(Function&& Func, Args&&... ArgList) -> decltype(std::forward<Function>(std::forward<Args>(ArgList)...))
{
	return std::forward<Function>(Func)(std::forward<Args>(ArgList)...);
}

static constexpr size_t Kilobyte = 1 << 10u;
static constexpr size_t Megabyte = 1 << 20u;
static constexpr size_t Gigabyte = 1 << 30u;

class NoneCopyable
{
public:
	NoneCopyable() = default;
	~NoneCopyable() = default;

	NoneCopyable(const NoneCopyable&) = delete;
	NoneCopyable& operator=(const NoneCopyable&) = delete;
protected:
private:
};

template<class T, class Any>
inline T* Cast(Any* Ptr) { return static_cast<T*>(Ptr); }

template<class T, class Any>
inline const T* Cast(const Any* Ptr) { return static_cast<const T*>(Ptr); }

template<class T, class Any>
inline std::shared_ptr<T> Cast(const std::shared_ptr<Any>& SharedPtrRef) { return std::static_pointer_cast<T>(SharedPtrRef); }

template<class T, class Any>
inline std::shared_ptr<T> Cast(std::shared_ptr<Any>& SharedPtrRef) { return std::static_pointer_cast<T>(SharedPtrRef); }

template<class T, class Any>
inline T& Cast(Any& Ref) { return static_cast<T&>(Ref); }

template<class T, class Any>
inline const T& Cast(const Any& Ref) { return static_cast<const T&>(Ref); }

template<class T>
struct Hasher
{
	size_t Hash = 0u;

	inline bool operator==(const Hasher& Other) const { return Other.Hash == Hash; }
	inline bool operator!=(const Hasher& Other) const { return Other.Hash != Hash; }

	Hasher() = default;

	Hasher(const T& Object)
		: Hash(std::hash<T>()(Object))
	{
	}
};

template<class T, class Enum>
class Array : public std::array<T, static_cast<size_t>(Enum::Num)>
{
public:
	inline T& operator[](Enum Index) { return this->at(static_cast<size_t>(Index)); }
	inline const T& operator[](Enum Index) const { return this->at(static_cast<size_t>(Index)); }

	inline T& operator[](size_t Index) { return this->at(Index); }
	inline const T& operator[](size_t Index) const { return this->at(Index); }
};

namespace Utils
{
	size_t PopulationCount(size_t Value);
	size_t GetPowerOfTwo(uint32_t Value);
	std::string GetTimepointString(const std::chrono::system_clock::time_point& Timepoint, const char* Format = "%Y-%m-%d %H:%M:%S");
}