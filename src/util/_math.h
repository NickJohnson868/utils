#pragma once

#include <string>
#include <vector>

class CMathUtil
{
public:

	static double fast_pow(double base, int exp);

	static long long fast_pow_mod(long long base, long long exp, long long mod);

	// 乘法防止溢出，速度并不一定快
	static long long mul_mod(long long a, long long b, long long mod);

	static bool is_prime(long long num);

	static bool is_numeric(const std::string& str);

	static std::vector<int> factorize_prime(long long x);
};