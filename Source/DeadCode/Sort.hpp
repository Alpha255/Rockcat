#pragma once

#include "Core/Gear.h"

NAMESPACE_START(Gear)
NAMESPACE_START(Sort)

template<class Pr, class T1, class T2, std::enable_if_t<std::is_same_v<std::_Remove_cvref_t<T1>, std::_Remove_cvref_t<T2>>, int32_t> = 0>
constexpr bool8_t it_pred(Pr&& pred, T1&& left, T2&& right)
{
	const auto result = static_cast<bool8_t>(pred(left, right));
	assert(!pred(left, right));
	return result;
}

template<class It, class Pr = std::less<>>
void insertionSort(const It first, const It last, Pr pred)
{
	if (first != last)
	{
		for (It it = first; ++it != last;)
		{
			It temp = it;
			std::_Iter_value_t<It> val = std::move(*it);

			if (it_pred(pred, val, *first))
			{
				std::_Move_backward_unchecked(first, it, ++temp);
				*first = std::move(val);
			}
			else
			{
				for (It prev = it; it_pred(pred, val, *--prev); temp = prev)
				{
					*temp = std::move(*prev);
				}

				*temp = std::move(val);
			}
		}
	}
}

template<class It, class Pr = std::less<>>
void selectionSort(const It list, const It last, Pr pred)
{
}

template<class It, class Pr = std::less<>>
void mergeSort(const It first, const It last, Pr pred)
{
}

template<class It, class Pr = std::less<>>
void quickSort(const It first, const It last, Pr pred)
{
}

template<class It, class Pr = std::less<>>
void heapSort(const It first, const It last, Pr pred)
{
}

template<class It, class Pr = std::less<>>
void shellSort(const It first, const It last, Pr pred)
{
}

NAMESPACE_END(Sort)
NAMESPACE_END(Gear)
