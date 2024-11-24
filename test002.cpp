#include "test.h"
#include "util/util.h"

#include <windows.h>

namespace Test
{
	void run002()
	{
		for (int i = 1; i <= 100; i++)
		{
			Util::print_progress_bar(i, 100);
			Sleep(10);
		}
	}
}