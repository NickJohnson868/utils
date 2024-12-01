#include "_math.h"

#include <regex>
#include <cmath>

#define SQRT_LL 3037000499LL

double CMathUtil::fast_pow(double base, int exp)
{
	double result = 1.0;
	while (exp > 0)
	{
		if (exp & 1)
			result *= base;

		base *= base;
		exp = exp >> 1;
	}
	return result;
}

long long CMathUtil::fast_pow_mod(long long base, long long exp, long long mod)
{
	long long result = 1;
	base = base % mod;
	while (exp > 0)
	{
		if (exp & 1)
			result = (result * base) % mod;

		if (base >= SQRT_LL)
			base = mul_mod(base, base, mod); // ¿ØÖÆÒç³ö
		else
			base = base * base % mod;
		exp = exp >> 1;
	}
	return result;
}

long long CMathUtil::mul_mod(long long a, long long b, long long mod)
{
	long long result = 0;
	a = a % mod;
	b = b % mod;

	while (b)
	{
		if (b & 1)
			result = (result + a) % mod;

		a = (a + a) % mod;
		b >>= 1;
	}

	return result;
}

bool CMathUtil::is_prime(long long n)
{
	if (n <= 1) return false;
	if (n <= 3) return true;
	if (n % 2 == 0 || n % 3 == 0) return false;
	long long x = std::sqrt(n);
	for (long long i = 5; i <= x; i += 6) {
		if (n % i == 0 || n % (i + 2) == 0) {
			return false;
		}
	}
	return true;
}

bool CMathUtil::is_numeric(const std::string& str)
{
	std::regex pattern(R"(^[+-]?(\d+(\.\d*)?|\.\d+)([eE][+-]?\d+)?$)");
	return std::regex_match(str, pattern);
}

std::vector<int> CMathUtil::factorize_prime(long long x)
{
	std::vector<int> nums;
	while (x % 2 == 0)
	{
		nums.push_back(2);
		x /= 2;
	}

	for (int i = 3; i * i <= x; i += 2)
	{
		while (x % i == 0)
		{
			nums.push_back(i);
			x /= i;
		}
	}
	if (x > 1) nums.push_back(static_cast<int>(x));
	return nums;
}