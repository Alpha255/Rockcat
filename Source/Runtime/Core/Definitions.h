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
#define VERIFY_WITH_PLATFORM_MESSAGE(Condition)                                          \
{                                                                                        \
	if (!(Condition))                                                                    \
	{                                                                                    \
		LOG_ERROR("Failed to invoke platform API, {}", PlatformMisc::GetErrorMessage()); \
		assert(0);                                                                       \
	}                                                                                    \
}

#define DLL_EXTENSION ".dll"
#else
	#error Unknown platform!
#endif

#define RENDERER_POSTFIX "Renderer"

#define SPDLOG_LEVEL_NAMES                                                                                                                 \
    {                                                                                                                                      \
        "Trace", "Debug", "Info", "Warning", "Error", "Critical", "Off"                                                                    \
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

#define CEREAL_BASE(ClassType) cereal::make_nvp(typeid(ClassType).name(), cereal::virtual_base_class<ClassType>(this))

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

template<class TName, class TExtension>
inline std::filesystem::path GetFilePath(TName&& Name, TExtension&& Extension)
{
	return std::filesystem::path(std::forward<TName>(Name)).replace_extension(std::filesystem::path(std::forward<TExtension>(Extension)));
}

template<class TParentPath, class TName, class TExtension>
inline std::filesystem::path GetFilePath(TParentPath&& ParentPath, TName&& Name, TExtension&& Extension)
{
	return (std::filesystem::path(std::forward<TParentPath>(ParentPath)) / std::filesystem::path(std::forward<TName>(Name))).replace_extension(std::filesystem::path(std::forward<TExtension>(Extension)));
}

size_t PopulationCount(size_t Value);

std::string TrimEnumString(const std::string& Name);

void SplitEnumArgs(const char* Args, std::string Names[], uint32_t Max);

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
	static constexpr const char* ToString(Value& EnumName)  \
	{                                                          \
		static std::string Names[Max];                         \
		if (Names[0].empty())                                  \
		{                                                      \
			SplitEnumArgs(#__VA_ARGS__, Names, Max);           \
		}                                                      \
		return Names[static_cast<uint32_t>(EnumName)].c_str(); \
	}                                                          \
};

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
inline std::shared_ptr<T> Cast(std::shared_ptr<Any>&& SharedPtrRef) { return std::static_pointer_cast<T>(SharedPtrRef); }

template<class T, class Any>
inline T& Cast(Any& Ref) { return static_cast<T&>(Ref); }

template<class T, class Any>
inline const T& Cast(const Any& Ref) { return static_cast<const T&>(Ref); }