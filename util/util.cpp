#include "util.h"
#include "_string.h"

#include <random>
#include <windows.h>
#include <iostream>
#include <numeric>

void Util::set_cursor_info(bool visible)
{
	HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	cursorInfo.dwSize = 10;  // 光标的大小，范围是1到100
	cursorInfo.bVisible = visible;  // 光标是否可见
	SetConsoleCursorInfo(hConsoleOutput, &cursorInfo);
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

void Util::color_print(P_R color, const char* const _Format, ...)
{
	const char* colorCode = "";

	switch (color) {
	case _ERROR:
		colorCode = "\033[31m";  // 红色
		break;
	case _WARNING:
		colorCode = "\033[33m";  // 黄色
		break;
	case _INFO:
		colorCode = "\033[34m";  // 蓝色
		break;
	case _SUCCESS:
		colorCode = "\033[32m";  // 绿色
		break;
	}

	va_list args;
	va_start(args, _Format);
	printf("%s", colorCode);  // 打印颜色
	vprintf(_Format, args);   // 打印格式化内容
	printf("\033[0m");        // 重置颜色
	va_end(args);
}

void Util::color_print(const P_P& color, const char* const _Format, ...)
{
	unsigned char r = color.R;
	unsigned char g = color.G;
	unsigned char b = color.B;

	// 使用 va_list 来处理可变参数
	va_list args;
	va_start(args, _Format);

	// 设置前景色为传入的 RGB
	printf("\033[38;2;%d;%d;%dm", r, g, b);
	vprintf(_Format, args);   // 打印格式化内容
	printf("\033[0m");        // 重置颜色
	va_end(args);
}

void print_line(char chr, int len)
{
	for (int i = 0; i < len; i++)
		putchar(chr);
	printf("\n");
}

void Util::print_table(const Table_DATA& table)
{
	std::vector<int> col_width(table.col_name.size(), 0); // 存储每列的最大宽度

	// 计算每列的最大宽度
	for (size_t i = 0; i < table.col_name.size(); ++i) {
		col_width[i] = table.col_name[i].length();  // 初始化宽度为列名的长度
	}

	for (const auto& entry : table.data)
	{
		if (entry.first.length() > col_width[0]) col_width[0] = entry.first.length();
		for (size_t i = 1; i < table.col_name.size(); ++i) {
			int v_len = entry.second[i - 1].length(); // 获取当前行数据的长度
			if (v_len > col_width[i]) {
				col_width[i] = v_len; // 更新列的最大宽度
			}
		}
	}

	// 打印表头
	print_line('-', std::accumulate(col_width.begin(), col_width.end(), 0) + 3 * (col_width.size()) + 1);
	for (size_t i = 0; i < table.col_name.size(); ++i)
		printf("| %-*s ", col_width[i], table.col_name[i].data());
	printf("|\n");
	print_line('-', std::accumulate(col_width.begin(), col_width.end(), 0) + 3 * (col_width.size()) + 1);

	// 打印数据行
	for (const auto& entry : table.data)
	{
		Util::color_print(_SUCCESS, "| %-*s ", col_width[0], entry.first.data());
		for (size_t i = 1; i < table.col_name.size(); ++i)
			Util::color_print(_SUCCESS, "| %-*s ", col_width[i], entry.second[i - 1].data());
		Util::color_print(_SUCCESS, "|\n");
		print_line('-', std::accumulate(col_width.begin(), col_width.end(), 0) + 3 * (col_width.size()) + 1);
	}
}