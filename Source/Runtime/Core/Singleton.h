#pragma once

#include "Core/Definitions.h"

template<class T> 
class Singleton : public NoneCopyable
{
public:
	static T& Get()
	{
		static T Instance;
		return Instance;
	}

	Singleton(const Singleton&) = delete;
protected:
	Singleton() = default;
	virtual ~Singleton() = default;
};

template<class T> 
class LazySingleton : public NoneCopyable
{
public:
	template <class... TArgs> 
	static void Create(TArgs&&... Args)
	{
		if (!Instance)
		{
			Instance = std::unique_ptr<T>(new T(std::forward<TArgs>(Args)...));
			/// std::shared_ptr<T>(new T(args...)) may call a non-public constructor of T if executed in context where it is accessible, 
			/// while std::make_shared requires public access to the selected constructor.
		}
	}

	static void Destroy()
	{
		Instance.reset();
	}

	static T& Get()
	{
		assert(Instance);
		return *Instance;
	}
protected:
	LazySingleton() = default;
	virtual ~LazySingleton() = default;
private:
	static std::unique_ptr<T> Instance;
};
template <class T> std::unique_ptr<T> LazySingleton<T>::Instance;

#define ALLOW_ACCESS(ClassType) friend class Singleton<ClassType>;
#define ALLOW_ACCESS_LAZY(ClassType) friend class LazySingleton<ClassType>;

#define SINGLETON_DECLARE_TYPE(ClassType, SingletonType) \
class ClassType : public SingletonType<ClassType>        \
{                                                        \
	friend class SingletonType<ClassType>;

#define SINGLETON_DECLARE(ClassType) SINGLETON_DECLARE_TYPE(ClassType, Gear::Singleton)

#define SINGLETON_DECLARE_LAZY(ClassType) SINGLETON_DECLARE_TYPE(ClassType, Gear::LazySingleton)

class SingletonFactory : public Singleton<SingletonFactory>
{
public:
protected:
private:
	std::unordered_map<std::type_info, std::unique_ptr<NoneCopyable>> m_Singletons;
};

class ModuleFactory
{
};


