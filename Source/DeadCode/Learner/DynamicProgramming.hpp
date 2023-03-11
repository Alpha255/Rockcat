#pragma once

#include "Gear/Gear.h"

class Fibonacci
{
public:
	uint32_t operator()(uint32_t n)
	{

	}
protected:
	uint32_t Fibonacci_0(uint32_t n)
	{
		if (n == 0u)
		{
			return 0u;
		}
		if (n == 1u || n == 2u)
		{
			return 1u;
		}
		return Fibonacci_0(n - 1u) + Fibonacci_0(n - 2u);
	}

	uint32_t Fibonacci_1(uint32_t n)
	{
		if (n < 1u)
		{
			return 0u;
		}
		std::vector<uint32_t> memo(n + 1u, 0u);
		return Fibonacci_1_Internal(memo, n);
	}

	uint32_t Fibonacci_1_Internal(std::vector<uint32_t>& memo, uint32_t n)
	{
		if (n == 1u || n == 2u)
		{
			return 1u;
		}
		if (memo[n])
		{
			return memo[n];
		}
		memo[n] = Fibonacci_1_Internal(memo, n - 1u) + Fibonacci_1_Internal(memo, n - 2u);
		return memo[n];
	}

	uint32_t Fibonacci_2(uint32_t n)
	{
		std::vector<uint32_t> fib(n + 1u, 0u);
		fib[1] = fib[2] = 1u;
		for (uint32_t i = 3u; i <= n; ++i)
		{
			fib[i] = fib[i - 1u] + fib[i - 2u];
		}
		return fib[n];
	}

	uint32_t Fibonacci_3(uint32_t n)
	{

	}
};
