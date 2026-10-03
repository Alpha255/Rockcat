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
	template <class... Args> 
	static void Create(Args&&... InArgs)
	{
		if (!Instance)
		{
			Instance = std::unique_ptr<T>(new T(std::forward<Args>(InArgs)...));
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


