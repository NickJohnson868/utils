#include "_math.h"

double _Math::fast_pow(double base, int exp)
{
	double result = 1.0;
	while (exp > 0)
	{
		if (exp & 1)
			result *= base;
		exp = exp >> 1;
		base *= base;
	}
	return result;
}

long long _Math::fast_pow(long long base, long long exp, long long mod)
{
	long long result = 1;
	base = base % mod;
	while (exp > 0)
	{
		if (exp & 1)
			result = (result * base) % mod;
		base = (base * base) % mod;
		exp = exp >> 1;
	}
	return result;
}