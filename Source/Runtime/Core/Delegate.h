#pragma once

#include "Runtime/Core/Definitions.h"

/// From https://github.com/simco50/CppDelegates

#if __cplusplus >= 201703L
	#define NO_DISCARD [[nodiscard]]
#else
	#define NO_DISCARD
#endif

struct IDelegatesInternal
{
	template<bool8_t IsConst, class TObject, class TRet, class ...TArgs>
	struct MemberFunction;

	template<class TObject, class TRet, class ...TArgs>
	struct MemberFunction<true, TObject, TRet, TArgs...>
	{
		using Type = TRet(TObject::*)(TArgs...) const;
	};

	template<class TObject, class TRet, class ...TArgs>
	struct MemberFunction<false, TObject, TRet, TArgs...>
	{
		using Type = TRet(TObject::*)(TArgs...);
	};
};

template<class TRet, class ...TArgs>
class IDelegate
{
public:
	IDelegate() = default;
	virtual ~IDelegate() noexcept = default;

	virtual TRet Execute(TArgs&&... Args) = 0;
};

template<class TRet, class ...TArgs>
class StaticDelegate;

template<class TRet, class ...TArgs, class ...TArgs2>
class StaticDelegate<TRet(TArgs...), TArgs2...> : public IDelegate<TRet, TArgs...>
{
public:
	using Function = TRet(*)(TArgs..., TArgs2...);

	StaticDelegate(Function Executer, TArgs2&&... Args2)
		: m_Executer(Executer)
		, m_Args(std::forward<TArgs2>(Args2)...)
	{
	}

	StaticDelegate(Function Executer, const std::tuple<TArgs2...>& Args2)
		: m_Executer(Executer)
		, m_Args(Args2)
	{
	}

	TRet Execute(TArgs&&... Args) override final
	{
		return ExecuteInternal(std::forward<TArgs>(Args)..., std::index_sequence_for<TArgs2...>());
	}
private:
	Function m_Executer;
	std::tuple<TArgs2...> m_Args;

	template<size_t... ArgIndex>
	TRet ExecuteInternal(TArgs&&... Args, std::index_sequence<ArgIndex...>)
	{
		return m_Executer(std::forward<TArgs>(Args)..., std::get<ArgIndex>(m_Args)...);
	}
};

template<bool8_t IsConst, class T, class TRet, class ...TArgs>
class RawDelegate;

template<bool8_t IsConst, class T, class TRet, class ...TArgs, class ...TArgs2>
class RawDelegate<IsConst, T, TRet(TArgs...), TArgs2...> : public IDelegate<TRet, TArgs...>, private IDelegatesInternal
{
public:
	using Function = typename IDelegatesInternal::MemberFunction<IsConst, T, TRet, TArgs..., TArgs2...>::Type;

	RawDelegate(T* Object, Function Executer, TArgs2&&... Args2)
		: m_Object(Object)
		, m_Executer(Executer)
		, m_Args(std::forward<TArgs2>(Args2)...)
	{
	}

	RawDelegate(T* Object, Function Executer, const std::tuple<TArgs2...>& Args2)
		: m_Object(Object)
		, m_Executer(Executer)
		, m_Args(Args2)
	{
	}

	TRet Execute(TArgs&&... Args) override final
	{
		return ExecuteInternal(std::forward<TArgs>(Args)..., std::index_sequence_for<TArgs2...>());
	}
private:
	T* m_Object;
	Function m_Executer;
	std::tuple<TArgs2...> m_Args;

	template<size_t... ArgIndex>
	TRet ExecuteInternal(TArgs&&... Args, std::index_sequence<ArgIndex...>)
	{
		if (!m_Object)
		{
			return TRet();
		}

		return (m_Object->*m_Executer)(std::forward<TArgs>(Args)..., std::get<ArgIndex>(m_Args)...);
	}
};

template<class TLambda, class TRet, class ...TArgs>
class LambdaDelegate;

template<class TLambda, class TRet, class ...TArgs, class ...TArgs2>
class LambdaDelegate<TLambda, TRet(TArgs...), TArgs2...> : public IDelegate<TRet, TArgs...>
{
public:
	explicit LambdaDelegate(TLambda&& Lambda, TArgs2&&... Args)
		: m_Lambda(std::forward<TLambda>(Lambda))
		, m_Args(std::forward<TArgs2>(Args)...)
	{
	}

	explicit LambdaDelegate(TLambda&& Lambda, const std::tuple<TArgs2...>& Args)
		: m_Lambda(std::forward<TLambda>(Lambda))
		, m_Args(Args)
	{
	}

	TRet Execute(TArgs&&... Args) override final
	{
		return ExecuteInternal(std::forward<TArgs>(Args)..., std::index_sequence_for<TArgs2...>());
	}
private:
	TLambda m_Lambda;
	std::tuple<TArgs2...> m_Args;

	template<size_t... ArgIndex>
	TRet ExecuteInternal(TArgs&&... Args, std::index_sequence<ArgIndex...>)
	{
		return (TRet)((m_Lambda)(std::forward<TArgs>(Args)..., std::get<ArgIndex>(m_Args)...));
	}
};

template<bool8_t IsConst, class T, class TRet, class ...TArgs>
class SPDelegate;

template<bool8_t IsConst, class T, class TRet, class ...TArgs, class ...TArgs2>
class SPDelegate<IsConst, T, TRet(TArgs...), TArgs2...> : public IDelegate<TRet, TArgs...>, private IDelegatesInternal
{
public:
	using Function = typename IDelegatesInternal::MemberFunction<IsConst, T, TRet, TArgs..., TArgs2...>::Type;

	SPDelegate(std::shared_ptr<T> Object, Function Executer, TArgs2&&... Args)
		: m_Object(Object)
		, m_Executer(Executer)
		, m_Args(std::forward<TArgs2>(Args)...)
	{
	}

	SPDelegate(std::shared_ptr<T> Object, Function Executer, const std::tuple<TArgs2...>& Args)
		: m_Object(Object)
		, m_Executer(Executer)
		, m_Args(Args)
	{
	}

	TRet Execute(TArgs&&... Args) override final
	{
		return ExecuteInternal(std::forward<TArgs>(Args)..., std::index_sequence_for<TArgs2...>());
	}
private:
	std::weak_ptr<T> m_Object;
	Function m_Executer;
	std::tuple<TArgs2...> m_Args;

	template<size_t... ArgIndex>
	TRet ExecuteInternal(TArgs&&... Args, std::index_sequence<ArgsIndex...>)
	{
		if (m_Object.expired())
		{
			return TRet();
		}

		std::shared_ptr<T> Object = m_Object.lock();
		return (Object.get()->*m_Executer)(std::forward<TArgs>(Args)..., std::get<ArgIndex>(m_Args)...);
	}
};

class DelegateBase
{
public:
	constexpr DelegateBase() noexcept
	{
	}

	virtual ~DelegateBase() noexcept
	{
	}


};

template<class TRet, class ...TArgs>
class Delegate : private IDelegatesInternal
{
private:
	template<class T, class... TArgs2>
	using ConstMemberFunction = typename IDelegatesInternal::MemberFunction<true, T, TRet, TArgs..., TArgs2...>::Type;

	template<class T, class... TArgs2>
	using NonConstMemberFunction = typename IDelegatesInternal::MemberFunction<false, T, TRet, TArgs..., TArgs2...>::Type;
public:
	using DelegateType = IDelegate<TRet, TArgs...>;

	template<class T, class ...TArgs2>
	NO_DISCARD static Delegate CreateRaw(T* Object, NonConstMemberFunction<T, TArgs2...> Executer, TArgs2... Args)
	{
		Delegate Handler;
		Handler.Bind<RawDelegate<false, T, TRet(TArgs...), TArgs2...>>(Object, Executer, std::forward<TArgs2>(Args)...);
		return Handler;
	}

	template<class T, class ...TArgs2>
	NO_DISCARD static Delegate CreateRaw(T* Object, ConstMemberFunction<T, TArgs2...> Executer, TArgs2... Args)
	{
		Delegate Handler;
		Handler.Bind<RawDelegate<true, T, TRet(TArgs...), TArgs2...>>(Object, Executer, std::forward<TArgs2>(Args)...);
		return Handler;
	}

	template<class T, class ...TArgs2>
	NO_DISCARD static Delegate CreateStatic(TRet(*Executer)(TArgs..., TArgs2...), TArgs2... Args)
	{
		Delegate Handler;
		Handler.Bind<StaticDelegate<TRet(TArgs...), TArgs2...>>(Executer, std::forward<TArgs2>(Args)...);
		return Handler;
	}

	template<class T, class ...TArgs2>
	NO_DISCARD static Delegate CreateSP(const std::shared_ptr<T>& Object, NonConstMemberFunction<T, TArgs2...> Executer, TArgs2... Args)
	{
		Delegate Handler;
		Handler.Bind<SPDelegate<false, T, TRet(TArgs...), TArgs2...>>(Object, Executer, std::forward<TArgs2>(Args)...);
		return Handler;
	}

	template<class T, class ...TArgs2>
	NO_DISCARD static Delegate CreateSP(const std::shared_ptr<T>& Object, ConstMemberFunction<T, TArgs2...> Executer, TArgs2... Args)
	{
		Delegate Handler;
		Handler.Bind<SPDelegate<true, T, TRet(TArgs...), TArgs2...>>(Object, Executer, std::forward<TArgs2>(Args)...);
		return Handler;
	}

	template<class TLambda, class ...TArgs2>
	NO_DISCARD static Delegate CreateLambda(TLambda&& Lambda, TArgs2... Args)
	{
		Delegate Handler;
		Handler.Bind<LambdaDelegate<TLambda, TRet(TArgs...), TArgs2...>>(std::forward<TLambda>(Lambda), std::forward<TArgs2>(Args)...);
		return Handler;
	}

	template<class T, class... TArgs2>
	void BindRaw(T* Object, NonConstMemberFunction<T, TArgs2...> Executer, TArgs2&&... Args)
	{
		static_assert(!std::is_const<T>::value, "Cannot Bind a non-const Executer on a const Object!");
		*this = CreateRaw<T, TArgs2...>(Object, Executer, std::forward<TArgs2>(Args)...);
	}

	template<class T, class... TArgs2>
	void BindRaw(T* Object, ConstMemberFunction<T, TArgs...> Executer, TArgs2&&... Args)
	{
		*this = CreateRaw<T, TArgs2...>(Object, Executer, std::forward<TArgs2>(Args)...);
	}

	template<class ...TArgs2>
	void BindStatic(TRet(*Executer)(TArgs..., TArgs2...), TArgs2&&... Args)
	{
		*this = CreateStatic<TArgs2...>(Executer, std::forward<TArgs2>(Args)...);
	}

	template<class TLambda, class ...TArgs2>
	void BindLambda(TLambda&& Lambda, TArgs2&&... Args)
	{
		*this = CreateLambda<TLambda, TArgs2...>(std::forward<TLambda>(Lambda), std::forward<TArgs2>(Args)...);
	}

	template<class T, class ...TArgs2>
	void BindSP(std::shared_ptr<T> Object, NonConstMemberFunction<T, TArgs2...> Executer, TArgs2&&... Args)
	{
		static_assert(!std::is_const<T>::value, "Cannot Bind a non-const Executer on a const Object!");
		*this = CreateSP<T, TArgs2...>(Object, Executer, std::forward<TArgs2>(Args)...);
	}

	template<class T, class ...TArgs2>
	void BindSP(std::shared_ptr<T> Object, ConstMemberFunction<T, TArgs2...> Executer, TArgs2&&... Args)
	{
		*this = CreateSP<T, TArgs2...>(Object, Executer, std::forward<TArgs2>(Args)...);
	}

	bool8_t HasBound() const
	{
		return (m_Delegate);
	}

	TRet Execute(TArgs... Args) const
	{
		assert(m_Delegate);
		m_Delegate->Execute(std::forward<TArgs>(Args)...);
	}

	TRet ExecuteIfBound(TArgs... Args)
	{
		if (HasBound())
		{
			m_Delegate->Execute(std::forward<TArgs>(Args)...);
		}
	}
private:
	template<class T, class... TArgs>
	void Bind(TArgs&&... Args)
	{
		m_Delegate.reset();
		m_Delegate = std::make_unique<T>(std::forward<TArgs>(Args)...);
	}

	std::unique_ptr<DelegateType> m_Delegate;
};

