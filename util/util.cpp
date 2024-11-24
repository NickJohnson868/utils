#include "util.h"
#include "_string.h"

#include <random>
#include <windows.h>
#include <iostream>

void Util::set_cursor_info(bool visible)
{
	HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 10;  // 光标的大小，范围是1到100
	cursorInfo.bVisible = visible;  // 光标是否可见
	SetConsoleCursorInfo(hConsoleOutput, &cursorInfo);
}

std::string Util::generate_uuid(bool b_upper, bool b_delimiter)
{
	char buffer[64] = { 0 };
	GUID guid;
	CoCreateGuid(&guid);
	std::string format = "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X";
	if (!b_upper) {
		format = _String::replace_all<std::string>(format, "X", "x");
	}
	if (!b_delimiter) {
		format = _String::replace_all<std::string>(format, "-", "");
	}
	_snprintf_s(buffer, sizeof(buffer),
		format.data(),
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	return buffer;
}

void Util::print_progress_bar(int row, int total)
{
	const int bar_width = 50;
	double progress = static_cast<double>(row) / total;
	int pos = static_cast<int>(bar_width * progress);

	printf(" [");
	for (int i = 0; i < bar_width; ++i)
	{
		if (i < pos) printf("=");
		else if (i == pos) printf(">");
		else printf(" ");
	}

	progress *= 100.0;
	if (progress - 100.0 > 0.001) progress = 100;
	printf("] %.2f%%\r", progress);
	fflush(stdout);
}