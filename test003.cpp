#include "test.h"
#include "util/_time.h"

#include <iostream>
#include "util/_math.h"

#define LL long long

using namespace std;

namespace Test
{
	LL for_mult(LL base, LL exp, LL mod)
	{
		LL sum = 1;
		for (int i = 0; i < exp; i++)
			sum = sum * base % mod;
		return sum;
	}

	void run003()
	{
		auto res = _Time::time_execution<LL(LL, LL, LL)>(for_mult, 11, 671088637, 17);
		std::cout << res.duration << " " << res.result << std::endl;

		res = _Time::time_execution<LL(LL, LL, LL)>(_Math::fast_pow, 11, 671088637, 17);
		std::cout << res.duration << " " << res.result << std::endl;
	}
}