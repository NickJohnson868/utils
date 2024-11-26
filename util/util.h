#pragma once

#include <string>
#include <unordered_map>
#include <vector>

typedef enum PRINT_COLOR {
	_ERROR,
	_WARNING,
	_INFO,
	_SUCCESS
} P_R;

typedef struct PRINT_PIXEL {
	unsigned char R, G, B;
} P_P;

struct Table_DATA {
	std::vector<std::string> col_name;  // 存储列名
	std::unordered_map<std::string, std::vector<std::string>> data;  // 存储每行的数据
};

class Util
{
public:
	static void set_cursor_info(bool visible);
	static void print_progress_bar(int row, int total);
	static void color_print(P_R color, const char* const _Format, ...);
	static void color_print(const P_P& color, const char* const _Format, ...);

	static void print_table(const Table_DATA& table);
};