#include "test.h"
#include "util/_time.h"
#include "util/_math.h"

#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>
#include <cstring>
#include <functional>
#include <type_traits>

using namespace std;
using namespace std::chrono;

#define LL long long

LL          _pow_cache[15][15];
bool        _use_cache = 1;
bool        _is_sync = 1;
bool        _use_quick_pow = 1;
bool        _use_for_mult = 1;
vector<LL>  daffodils_arr;

namespace Test
{
	double for_mult(double base, int exp)
	{
		double sum = 1;
		for (int i = 0; i < exp; i++)
			sum *= base;
		return sum;
	}

	LL _pow(int base, int exp)
	{
		//_ASSERT(x > 0 && y > 0);
		if (!_use_cache)
		{
			if (_use_quick_pow) return CMathUtil::fast_pow(base, exp);
			else if (_use_for_mult) return for_mult(base, exp);
			return pow(base, exp);
		}
		if (base == 0 || base == 1) return base;
		if (_pow_cache[base][exp]) return _pow_cache[base][exp];

		if (_use_quick_pow) _pow_cache[base][exp] = CMathUtil::fast_pow(base, exp);
		else if (_use_for_mult) _pow_cache[base][exp] = for_mult(base, exp);
		else _pow_cache[base][exp] = pow(base, exp);

		return _pow_cache[base][exp];
	}

	LL sub_sum(LL x, int n)
	{
		LL sum = 0;
		while (x)
		{
			sum += _pow(x % 10, n);
			x /= 10;
		}
		return sum;
	}

	void _daffodils(LL a, LL b, int n)
	{
		for (LL i = a; i < b; i++)
			if (sub_sum(i, n) == i)
				daffodils_arr.emplace_back(i);
	}

	void daffodils(int n)
	{
		daffodils_arr.resize(0);
		memset(_pow_cache, 0, sizeof(_pow_cache));
		if (!_is_sync)
		{
			std::vector<std::thread> threads;
			for (int i = 1; i < 10; ++i)
				threads.emplace_back(_daffodils, _pow(10, n - 1) * i, _pow(10, n - 1) * (i + 1), n);

			for (auto& t : threads) t.join();
		}
		else
		{
			_daffodils(_pow(10, n - 1), _pow(10, n), n);
		}

		sort(daffodils_arr.begin(), daffodils_arr.end());
		for (auto value : daffodils_arr)
			cout << value << " ";
		cout << endl;
	}

	LL daffodils(int n, bool use_cache, bool is_sync, bool use_quick_pow, bool use_for_mult)
	{
		_use_cache = use_cache;
		_is_sync = is_sync;
		_use_quick_pow = use_quick_pow;
		_use_for_mult = use_for_mult;
		FunctionResult<void> res = CTimeUtil::time_execution<void(int), int>(daffodils, n);
		auto duration = res.duration;
		printf("daffodils(%d) [%s][%s][%s][%s] [%lld΢��][%.3lf%����]\n", n,
			_use_cache ? "����" : "������", _is_sync ? "ͬ��" : "10�߳�",
			_use_quick_pow ? "������" : "��ͨ��", _use_for_mult ? "����" : "������",
			duration.count(), duration.count() / 1000.0);
		return duration.count();
	}

	void test_daffodils()
	{
		auto rate = [](double base, double b)
			{
				//return (1.0 / b - 1.0 / base) / (1.0 / base) * 100;
				return (base / b - 1) * 100;
			};

		for (int i = 6; i <= 6; i++)
		{
			double a = daffodils(i, false, true, false, false);
			double b = daffodils(i, false, false, false, false); // ʹ��10�߳�
			double c = daffodils(i, true, true, false, false); // ʹ�û���
			double d = daffodils(i, false, true, true, false); // ʹ�ÿ�����
			double e = daffodils(i, false, true, false, true); // ʹ������
			double f = daffodils(i, true, false, true, false); // ����
			printf("daffodils(%d) ʹ��10�߳̿��� %.2lf%%\n", i, rate(a, b));
			printf("daffodils(%d) ʹ�û������ %.2lf%%\n", i, rate(a, c));
			printf("daffodils(%d) ʹ�ÿ����ݿ��� %.2lf%%\n", i, rate(a, d));
			printf("daffodils(%d) ʹ�����˿��� %.2lf%%\n", i, rate(a, e));
			printf("daffodils(%d) ȫʹ��(������)���� %.2lf%%\n\n", i, rate(a, f));
		}
	}
}