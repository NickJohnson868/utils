#pragma once

#include <string>
#include <vector>

enum PrintColor {
	P_ERROR,
	P_WARNING,
	P_INFO,
	P_SUCCESS,
	P_NORMAL
};

typedef struct RGB {
	unsigned char R, G, B;
} RGB;

typedef struct RGBA {
	unsigned char R, G, B, A;
} RGBA;

typedef std::vector<std::vector<std::string> > Table_Data;

class Util {
public:
	static void set_cursor_visible(bool visible);

	static bool is_cursor_visible();

	static void print_progress_bar(long long row, long long total, const char* title = "");

	static void color_print(PrintColor color, const char* Format, ...);

	static void color_print(RGB color, const char* Format, ...);

	static void print_table(const Table_Data& table);

	static bool get_cmd_output(const std::string& command, std::string& output);

	static RGB rgba_to_rgb(const RGBA& rgba, const RGB& bk);
};
