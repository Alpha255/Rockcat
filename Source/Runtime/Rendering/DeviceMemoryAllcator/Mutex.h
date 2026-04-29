#pragma once

#include "Core/Definitions.h"
#include <synchapi.h>

#define DMA_NO_COPY(Class) \
private: \
	Class(const Class&) = delete; \
	Class& operator=(const Class&) = delete;

#define DMA_NO_COPY_NO_MOVE(Class) \
private: \
	Class(const Class&) = delete; \
	Class(Class&&) = delete; \
	Class& operator=(const Class&) = delete; \
	Class& operator=(Class&&) = delete;

#define DMA_FORCE_INLINE __forceinline

#ifndef  DMA_USE_SHARED_MUTEX
	#if __cplusplus >= 201703L || _MSVC_LANG >= 201703L
		#define DMA_USE_SHARED_MUTEX 1
	#elif defined(_MSC_FULL_VER) && _MSC_FULL_VER >= 190023918 && __cplusplus == 199711L && _MSVC_LANG >= 201703L
		#define DMA_USE_SHARED_MUTEX 1
	#else
		#define DMA_USE_SHARED_MUTEX 0
	#endif
#endif // ! DMA_USE_SHARED_MUTEX

namespace DMA
{
	enum class ESuballocationType
	{
		Unknown,
		Buffer,
		Image_Unknown,
		Image_Linear,
		Image_Optimal
	};

#if DMA_USE_SHARED_MUTEX
	class RWMutexShared
	{
		DMA_NO_COPY_NO_MOVE(RWMutexShared);
	public:
		RWMutexShared() = default;

		DMA_FORCE_INLINE void LockRead() { m_Mutex.lock_shared(); }
		DMA_FORCE_INLINE void UnlockRead() { m_Mutex.unlock_shared(); }
		DMA_FORCE_INLINE bool TryLockRead() { return m_Mutex.try_lock_shared(); }

		DMA_FORCE_INLINE void LockWrite() { m_Mutex.lock(); }
		DMA_FORCE_INLINE void UnlockWrite() { m_Mutex.unlock(); }
		DMA_FORCE_INLINE bool TryLockWrite() { return m_Mutex.try_lock(); }
	private:
		std::shared_mutex m_Mutex;
	};

	using RWMutex = RWMutexShared;
#elif defined(_WIN32) && defined(WINVER) && defined(SRWLOCK_INIT) && WINVER >= 0x0600
	class RWMutexSlim
	{
		DMA_NO_COPY_NO_MOVE(RWMutexSlim);
	public:
		RWMutexSlim() { InitializeSRWLock(&m_Lock); }

		DMA_FORCE_INLINE void LockRead() { AcquireSRWLockShared(&m_Lock); }
		DMA_FORCE_INLINE void UnlockRead() { ReleaseSRWLockShared(&m_Lock); }
		DMA_FORCE_INLINE bool TryLockRead() { return TryAcquireSRWLockShared(&m_Lock); }

		DMA_FORCE_INLINE void LockWrite() { AcquireSRWLockExclusive(&m_Lock); }
		DMA_FORCE_INLINE void UnlockWrite() { ReleaseSRWLockExclusive(&m_Lock); }
		DMA_FORCE_INLINE bool TryLockWrite() { return TryAcquireSRWLockExclusive(&m_Lock); }
	private:
		SRWLOCK m_Lock;
	};

	using RWMutex = RWMutexSlim;
#else
	class RWMutexDefault
	{
		DMA_NO_COPY_NO_MOVE(RWMutexDefault);
	public:
		RWMutexDefault() = default;

		DMA_FORCE_INLINE void LockRead() { m_Mutex.lock(); }
		DMA_FORCE_INLINE void UnlockRead() { m_Mutex.unlock(); }
		DMA_FORCE_INLINE bool TryLockRead() { return m_Mutex.try_lock(); }

		DMA_FORCE_INLINE void LockWrite() { m_Mutex.lock(); }
		DMA_FORCE_INLINE void UnlockWrite() { m_Mutex.unlock(); }
		DMA_FORCE_INLINE bool TryLockWrite() { return m_Mutex.try_lock(); }
	private:
		std::mutex m_Mutex;
	};

	using RWMutex = RWMutexDefault;
#endif
}
