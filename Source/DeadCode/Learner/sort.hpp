#pragma once

#include "Gear/Gear.h"

NAMESPACE_START(FORT)

/// O(N^2) steady
template <typename T> void InsertSort(T *pBegin, T *pEnd, std::function<bool8_t(const T &, const T &)> compFunc = [](const T &left, const T &right)->bool8_t {
	return left <= right;
})
{
	ptrdiff_t length = std::distance(pBegin, pEnd);
	uint32_t begin = 1U;
	int32_t i = 0U;

	while (begin < length)
	{
		T key = pBegin[begin];
		for (i = begin - 1; i >= 0; --i)
		{
			if (compFunc(pBegin[i], key))
			{
				pBegin[i + 1] = key;
				break;
			}
			pBegin[i + 1] = pBegin[i];
		}

		if (i < 0)
		{
			pBegin[0] = key;
		}

		++begin;
	}
}

/// O(n^1.3) not steady
template <typename T> void ShellSort(T *pBegin, T *pEnd, std::function<bool8_t(const T &, const T &)> compFunc = [](const T &left, const T &right)->bool8_t {
	return left <= right;
})
{
	ptrdiff_t length = std::distance(pBegin, pEnd);
	uint32_t incr = (uint32_t)length;

	while (incr > 1u)
	{
		incr = incr / 3u + 1u;
		for (uint32_t i = incr; i < length; ++i)
		{
			T key = pBegin[i];
			uint32_t start = i - incr;
			while (start >= 0u && start < length && compFunc(key, pBegin[start]))
			{
				pBegin[start + incr] = pBegin[start];
				start -= incr;
			}

			pBegin[start + incr] = key;
		}
	}
}

template <typename T> void QuickSort(T *pBegin, T *pEnd, std::function<bool8_t(const T &, const T &)> compFunc = [](const T &left, const T &right)->bool8_t {
	return left <= right;
})
{

}

template <typename T> void SelectSort(T *pBegin, T *pEnd, std::function<bool8_t(const T &, const T &)> compFunc = [](const T &left, const T &right)->bool8_t {
	return left <= right;
})
{

}

template <typename T> void HeapSort(T *pBegin, T *pEnd, std::function<bool8_t(const T &, const T &)> compFunc = [](const T &left, const T &right)->bool8_t {
	return left <= right;
})
{

}

NAMESPACE_END(FORT)
