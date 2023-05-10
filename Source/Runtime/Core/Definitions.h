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
///#include <span>  /// cpp20

#include <cereal/cereal.hpp>

#if defined(_WIN32)
	#if !defined(VK_USE_PLATFORM_WIN32_KHR)
		#define VK_USE_PLATFORM_WIN32_KHR 1
	#endif
	#if !defined(PLATFORM_WIN32)
		#define PLATFORM_WIN32 1
	#endif
#elif defined(__ANDRIOD__)
	#define VK_USE_PLATFORM_ANDROID_KHR 1
	#define PLATFORM_ANDROID 1
#endif

#if _HAS_CXX20
	#define MEM(MemberName) MemberName
#else
	#define MEM(MemberName)
#endif

#pragma warning(disable : 26812)

#if defined(max)
	#undef max
#endif

#if defined(min)
	#undef min
#endif

using char8_t = char;
using uchar8_t = unsigned char;
using float32_t = float;
using double64_t = double;
using bool8_t = bool;
using long32_t = long;
using ulong32_t = unsigned long;
using long64_t = long long;
using ulong64_t = unsigned long long;
using byte8_t = unsigned char;

static_assert(sizeof(char8_t) == 1ull, "Size of char miss match.");
static_assert(sizeof(uchar8_t) == 1ull, "Size of unsigned char miss match.");
static_assert(sizeof(float32_t) == 4ull, "Size of float miss match.");
static_assert(sizeof(double64_t) == 8ull, "Size of double miss match.");
static_assert(sizeof(bool8_t) == 1ull, "Size of bool miss match.");
static_assert(sizeof(long32_t) == 4ull, "Size of long miss match.");
static_assert(sizeof(long64_t) == 8ull, "Size of long long miss match.");
static_assert(sizeof(ulong32_t) == 4ull, "Size of long miss match.");
static_assert(sizeof(ulong64_t) == 8ull, "Size of long long miss match.");
static_assert(sizeof(byte8_t) == 1ull, "Size of byte miss match.");

#define NAMESPACE_START(Name) namespace Name {
#define NAMESPACE_END(Name) }

#if defined(DYNAMIC_LIB)
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
#define VERIFY_PLATFORM(Condition)                                                   \
{                                                                                    \
	if (!(Condition))                                                                \
	{                                                                                \
		assert(0);                                                                   \
		LOG_ERROR("Failed to invoke WINAPI, {}", Gear::Platform::GetErrorMessage()); \
	}                                                                                \
}

#define DLL_POSTFIX ".dll"
#else
	#error Unknown platform!
#endif

#define RENDERER_POSTFIX "Renderer"

#define DECLARE_SMART_PTR(ClassName) class ClassName; \
	using ClassName##SharedPtr = std::shared_ptr<ClassName>; \
	using ClassName##UniquePtr = std::unique_ptr<ClassName>; \
	using ClassName##WeakPtr = std::weak_ptr<ClassName>;

#define DESCRIPTION(Description)

#define TO_STRING(X) #X
#define CAT(A, B) A##B
#define FILE_LINE CAT(__FILE__, __LINE__)

#define CEREAL_BASE(ClassType) cereal::make_nvp(typeid(ClassType).name(), cereal::virtual_base_class<ClassType>(this))

#define ENUM_FLAG_OPERATORS(Enum) \
	inline constexpr Enum    operator|(Enum Left, Enum Right) { return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(Left) | static_cast<std::underlying_type_t<Enum>>(Right)); } \
	inline constexpr Enum    operator&(Enum Left, Enum Right) { return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(Left) & static_cast<std::underlying_type_t<Enum>>(Right)); } \
	inline constexpr Enum    operator^(Enum Left, Enum Right) { return static_cast<Enum>(static_cast<std::underlying_type_t<Enum>>(Left) ^ static_cast<std::underlying_type_t<Enum>>(Right)); } \
	inline constexpr Enum    operator~(Enum Value) { return static_cast<Enum>(~(static_cast<std::underlying_type_t<Enum>>(Value))); } \
	inline constexpr bool8_t operator!(Enum Value) { return !(static_cast<std::underlying_type_t<Enum>>(Value)); } \
	inline constexpr bool8_t operator!=(Enum Left, Enum Right) { return static_cast<int64_t>(Left) != static_cast<int64_t>(Right); } \
	inline constexpr bool8_t operator==(Enum Left, Enum Right) { return static_cast<int64_t>(Left) == static_cast<int64_t>(Right); }

template<class Enum>
constexpr bool8_t EnumHasAnyFlags(Enum Flags, Enum Contains)
{
	static_assert(std::is_enum_v<Enum>, "Must be enum!");
	return (static_cast<std::underlying_type_t<Enum>>(Flags) & static_cast<std::underlying_type_t<Enum>>(Contains)) != static_cast<std::underlying_type_t<Enum>>(0);
}

template<class Enum1, class Enum2>
constexpr bool8_t EnumHasAnyFlags(Enum1 Flags, Enum2 Contains)
{
	static_assert(std::is_enum_v<Enum1> || std::is_enum_v<Enum2>, "Either Enum1 or Enum2 be enum!");
	return (static_cast<int32_t>(Flags) & static_cast<int32_t>(Contains)) != 0;
}

template<class Enum>
constexpr bool8_t EnumHasAllFlags(Enum Flags, Enum Contains)
{
	static_assert(std::is_enum_v<Enum>, "Must be enum!");
	return (static_cast<std::underlying_type_t<Enum>>(Flags) & static_cast<std::underlying_type_t<Enum>>(Contains)) == static_cast<std::underlying_type_t<Enum>>(Contains);
}

template<class Enum1, class Enum2>
constexpr bool8_t EnumHasAllFlags(Enum1 Flags, Enum2 Contains)
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
inline bool8_t IsEqual(const Left& A, const Right& B)
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

template <class T>
inline constexpr bool8_t IsPowerOfTwo(T Value)
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

size_t PopulationCount(size_t Value);

std::string TrimEnumString(const std::string& Name);

void SplitEnumArgs(const char8_t* Args, std::string Names[], uint32_t Max);

static constexpr unsigned long long Kilobyte = 1 << 10ull;
static constexpr unsigned long long Megabyte = 1 << 20ull;
static constexpr unsigned long long Gigabyte = 1 << 30ull;

#define DECLARE_ENUM_CLASS(ClassName, ...)                     \
class ClassName                                                \
{                                                              \
public:                                                        \
	enum Value                                                 \
	{                                                          \
		__VA_ARGS__,                                           \
		Max                                                    \
	};                                                         \
	static constexpr const char8_t* ToString(Value& EnumName)  \
	{                                                          \
		static std::string Names[Max];                         \
		if (Names[0].empty())                                  \
		{                                                      \
			SplitEnumArgs(#__VA_ARGS__, Names, Max);           \
		}                                                      \
		return Names[static_cast<uint32_t>(EnumName)].c_str(); \
	}                                                          \
};


class NamedObject
{
public:
	NamedObject() = default;

	NamedObject(const char8_t* Name)
		: m_Name(Name)
	{
	}

	void SetName(const char8_t* Name)
	{
		m_Name = Name;
	}

	const char8_t* GetName() const noexcept
	{
		return m_Name.data();
	}
protected:
private:
	std::string m_Name;
};

#define SERIALIZE_START template<class Archive> \
	void serialize(Archive& Ar) \
	{ \
		Ar( \

#define SERIALIZE_END ); \
	}